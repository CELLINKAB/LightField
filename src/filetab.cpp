#include "pch.h"

#include "filetab.h"

#include "canvas.h"
#include "loader.h"
#include "mesh.h"
#include "printjob.h"
#include "processrunner.h"
#include "shepherd.h"
#include "strings.h"
#include "utils.h"

namespace {

    QRegularExpression VolumeLineMatcher { QString { "^\\s*volume\\s*[:=]\\s*(\\d+(?:\\.(?:\\d+))?)" }, QRegularExpression::CaseInsensitiveOption };

}

FileTab::FileTab( QWidget* parent ): TabBase( parent ) {
    debug( "+ FileTab::`ctor: construct at %p\n", this );

    _userMediaPath = MediaRootPath + Slash + GetUserName( );
    debug( "  + user media path '%s'\n", _userMediaPath.toUtf8( ).data( ) );

    _usbRetryTimer->setInterval( 1000 );
    _usbRetryTimer->setSingleShot( false );
    _usbRetryTimer->setTimerType( Qt::PreciseTimer );

    _currentFsModel = _libraryFsModel;

    _libraryFsModel->setFilter( QDir::Files );
    _libraryFsModel->setNameFilterDisables( false );
    _libraryFsModel->setNameFilters( { { "*.stl" } } );
    _libraryFsModel->setRootPath( StlModelLibraryPath );
    QObject::connect( _libraryFsModel, &QFileSystemModel::directoryLoaded, this, &FileTab::libraryFsModel_directoryLoaded );

    _usbFsModel->setFilter( QDir::Files );
    _usbFsModel->setNameFilterDisables( false );
    _usbFsModel->setNameFilters( { { "*.stl" } } );
    QObject::connect( _usbFsModel, &QFileSystemModel::directoryLoaded, this, &FileTab::usbFsModel_directoryLoaded );

    QObject::connect( _fsWatcher, &QFileSystemWatcher::directoryChanged, this, &FileTab::_checkUsbPath );
    _fsWatcher->addPath( MediaRootPath );

    _checkUserMediaPath( );

    _availableFilesLabel->setText( "Models in library:" );

    _availableFilesListView->setFlow( QListView::TopToBottom );
    _availableFilesListView->setLayoutMode( QListView::SinglePass );
    _availableFilesListView->setMovement( QListView::Static );
    _availableFilesListView->setResizeMode( QListView::Fixed );
    _availableFilesListView->setViewMode( QListView::ListMode );
    _availableFilesListView->setModel( _libraryFsModel );
    _availableFilesListView->grabGesture( Qt::SwipeGesture );
    QObject::connect( _availableFilesListView, &QListView::clicked, this, &FileTab::availableFilesListView_clicked );

    _toggleLocationButton->setEnabled( false );
    _toggleLocationButton->setText( "Show USB stick" );
    QObject::connect( _toggleLocationButton, &QPushButton::clicked, this, &FileTab::toggleLocationButton_clicked );

    _availableFilesLayout->setContentsMargins( { } );
    _availableFilesLayout->addWidget( _toggleLocationButton,   0, 0 );
    _availableFilesLayout->addWidget( _availableFilesLabel,    1, 0 );
    _availableFilesLayout->addWidget( _availableFilesListView, 2, 0 );

    _availableFilesContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _availableFilesContainer->setLayout( _availableFilesLayout );

    _selectButton->setEnabled( false );
    _selectButton->setFixedSize( MainButtonSize );
    _selectButton->setFont( ModifyFont( _selectButton->font( ), 22.0 ) );
    _selectButton->setText( "Select" );
    QObject::connect( _selectButton, &QPushButton::clicked, this, &FileTab::selectButton_clicked );

    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    format.setVersion( 2, 1 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    QSurfaceFormat::setDefaultFormat( format );

    _canvas = new Canvas( format, this );
    _canvas->setMinimumWidth( MaximalRightHandPaneSize.width( ) );
    _canvas->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    _dimensionsErrorLabel->hide( );
    _dimensionsErrorLabel->setPalette( ModifyPalette( _dimensionsErrorLabel->palette( ), QPalette::WindowText, Qt::red ) );
    _dimensionsErrorLabel->setText( "Model exceeds build volume!" );

    _dimensionsLayout = WrapWidgetsInHBox( { _dimensionsLabel, nullptr, _dimensionsErrorLabel } );
    _dimensionsLayout->setAlignment( Qt::AlignLeft );
    _dimensionsLayout->setContentsMargins( { } );

    _canvasLayout->setContentsMargins( { } );
    _canvasLayout->addWidget( _canvas );
    _canvasLayout->addLayout( _dimensionsLayout );

    _layout->setContentsMargins( { } );
    _layout->addWidget( _availableFilesContainer, 0, 0, 1, 1 );
    _layout->addWidget( _selectButton,            1, 0, 1, 1 );
    _layout->addLayout( _canvasLayout,            0, 1, 2, 1 );
    _layout->setRowStretch( 0, 4 );
    _layout->setRowStretch( 1, 1 );

    setLayout( _layout );
}

FileTab::~FileTab( ) {
    debug( "+ FileTab::`dtor: destruct at %p\n", this );
}

void FileTab::libraryFsModel_directoryLoaded( QString const& name ) {
    debug( "+ FileTab::libraryFsModel_directoryLoaded: name '%s'\n", name.toUtf8( ).data( ) );
    if ( _modelsLocation == ModelsLocation::Library ) {
        _libraryFsModel->sort( 0, Qt::AscendingOrder );
        _availableFilesListView->setRootIndex( _libraryFsModel->index( StlModelLibraryPath ) );
    }
}

void FileTab::usbFsModel_directoryLoaded( QString const& name ) {
    debug( "+ FileTab::usbFsModel_directoryLoaded: name '%s'\n", name.toUtf8( ).data( ) );
    if ( _modelsLocation == ModelsLocation::Usb ) {
        _usbFsModel->sort( 0, Qt::AscendingOrder );
        _availableFilesListView->setRootIndex( _usbFsModel->index( _usbPath ) );
    }
}

void FileTab::fsWatcher_directoryChanged( QString const& path ) {
    debug( "+ FileTab::fsWatcher_directoryChanged: path '%s'\n", path.toUtf8( ).data( ) );

    _checkUserMediaPath( );
}

void FileTab::usbRetryTimer_timeout( ) {
    debug( "+ FileTab::usbRetryTimer_timeout: retry count is %d\n", _usbRetryCount );

    _checkUserMediaPath( );

    if ( !_usbRetryTimer->isActive( ) ) {
        return;
    }

    --_usbRetryCount;
    if ( _usbRetryCount < 1 ) {
        debug( "+ FileTab::usbRetryTimer_timeout: out of retries, giving up\n" );
        _stopUsbRetry( );

        if ( _modelsLocation == ModelsLocation::Usb ) {
            _showLibrary( );
        }
        if ( _modelSelection.fileName.startsWith( _userMediaPath ) ) {
            _canvas->clear( );
            _dimensionsLabel->clear( );
            _selectButton->setEnabled( false );
            emit modelSelectionFailed( );
        }
        _toggleLocationButton->setEnabled( false );
    }
}

void FileTab::_checkUserMediaPath( ) {
    debug( "+ FileTab::_checkUserMediaPath\n" );

    QFileInfo userMediaPathInfo { _userMediaPath };
    if ( !userMediaPathInfo.exists( ) ) {
        debug( "  + User media path doesn't exist\n" );
        _fsWatcher->removePath( _userMediaPath );
        _startUsbRetry( );
        return;
    }

    if ( !userMediaPathInfo.isReadable( ) || !userMediaPathInfo.isExecutable( ) ) {
        debug( "  + User media path is inaccessible (uid: %u; gid: %u; mode: %04o)\n", userMediaPathInfo.ownerId( ), userMediaPathInfo.groupId( ), userMediaPathInfo.permissions( ) & 0777 );
    }

    if ( !_fsWatcher->directories( ).contains( _userMediaPath ) && !_fsWatcher->addPath( _userMediaPath ) ) {
        debug( "  + QFileSystemWatcher::addPath failed for user media path\n" );
        _startUsbRetry( );
        return;
    }

    _checkUsbPath( );
}

void FileTab::_checkUsbPath( ) {
    debug( "+ FileTab::_checkUsbPath\n" );

    QString dirname { GetFirstDirectoryIn( _userMediaPath ) };
    if ( dirname.isEmpty( ) ) {
        debug( "  + no directories in user media path\n" );
        _startUsbRetry( );
        return;
    }

    _usbPath = _userMediaPath + Slash + dirname;
    debug( "  + mounted USB device is '%s'\n", _usbPath.toUtf8( ).data( ) );

    QFileInfo usbPathInfo { _usbPath };
    if ( !usbPathInfo.isReadable( ) || !usbPathInfo.isExecutable( ) ) {
        debug( "  + USB path is inaccessible (uid: %u; gid: %u; mode: %04o)\n", usbPathInfo.ownerId( ), usbPathInfo.groupId( ), usbPathInfo.permissions( ) & 0777 );
        _startUsbRetry( );
        return;
    }

    debug( "  + USB path is good\n" );
    _stopUsbRetry( );

    _usbFsModel->setRootPath( _usbPath );
    _toggleLocationButton->setEnabled( true );
}

void FileTab::_startUsbRetry( ) {
    debug( "+ FileTab::_startUsbRetry\n" );
    if ( !_usbRetryTimer->isActive( ) ) {
        QObject::connect( _usbRetryTimer, &QTimer::timeout, this, &FileTab::usbRetryTimer_timeout );
        _usbRetryCount = 5;
        _usbRetryTimer->start( );
    }
}

void FileTab::_stopUsbRetry( ) {
    debug( "+ FileTab::_stopUsbRetry: stopping timer\n" );
    QObject::disconnect( _usbRetryTimer, nullptr, this, nullptr );
    _usbRetryTimer->stop( );
}

void FileTab::availableFilesListView_clicked( QModelIndex const& index ) {
    int indexRow = index.row( );
    if ( _selectedRow == indexRow ) {
        return;
    }

    _modelSelection = { ( ( _modelsLocation == ModelsLocation::Library ) ? StlModelLibraryPath : _usbPath ) + Slash + index.data( ).toString( ), };
    _selectedRow    = indexRow;
    debug( "+ FileTab::availableFilesListView_clicked: selection changed to row %d, file name '%s'\n", _selectedRow, _modelSelection.fileName.toUtf8( ).data( ) );

    _selectButton->setEnabled( false );
    _availableFilesListView->setEnabled( false );

    if ( _processRunner ) {
        QObject::disconnect( _processRunner, nullptr, this, nullptr );
        _processRunner->terminate( );
        _processRunner->deleteLater( );
        _processRunner = nullptr;
    }

    if ( !_loadModel( _modelSelection.fileName ) ) {
        debug( "  + _loadModel failed!\n" );
        _availableFilesListView->setEnabled( true );
    }
}

void FileTab::availableFilesListView_swipeGesture( QGestureEvent* event, QSwipeGesture* gesture ) {
    debug(
        "+ FileTab::availableFilesListView_swipeGesture:\n"
        "  + state:               %s [%d]\n"
        "  + horizontal direction %s [%d]\n"
        "  + vertical direction   %s [%d]\n"
        "",
        ToString( gesture->state( ) ),               gesture->state( ),
        ToString( gesture->horizontalDirection( ) ), gesture->horizontalDirection( ),
        ToString( gesture->verticalDirection( ) ),   gesture->verticalDirection( ),
        ToString( gesture->hasHotSpot( ) )
    );
    event->accept( );

    if ( !gesture->hasHotSpot( ) ) {
        debug( "  + gesture doesn't have a hotspot?\n" );
        return;
    }

    debug( "  + gesture hotspot:     %s\n", ToString( gesture->hotSpot( ) ) );
    switch ( gesture->state( ) ) {
        case Qt::GestureStarted:
            _swipeLastPoint = gesture->hotSpot( );
            break;

        case Qt::GestureUpdated:
            debug( "  + difference from previous position: %f\n", _swipeLastPoint.y( ) - gesture->hotSpot( ).y( ) );
            _swipeLastPoint = gesture->hotSpot( );
            break;

        case Qt::GestureFinished:
            debug( "  + difference from previous position: %f\n", _swipeLastPoint.y( ) - gesture->hotSpot( ).y( ) );
            _swipeLastPoint = gesture->hotSpot( );
            break;

        case Qt::GestureCanceled:
            break;

        case Qt::NoGesture:
            break;
    }
}

void FileTab::toggleLocationButton_clicked( bool ) {
    if ( _modelsLocation == ModelsLocation::Library ) {
        _showUsbStick( );
    } else {
        _showLibrary( );
    }
}

void FileTab::selectButton_clicked( bool ) {
    debug( "+ FileTab::selectButton_clicked\n" );
    auto selection = new ModelSelectionInfo( _modelSelection );
    emit modelSelected( selection );
    delete selection;
}

void FileTab::loader_gotMesh( Mesh* m ) {
    if ( _modelSelection.fileName.isEmpty( ) || ( _modelSelection.fileName[0].unicode( ) == L':' ) ) {
        debug( "+ FileTab::loader_gotMesh: file name '%s' is empty or resource name\n", _modelSelection.fileName.toUtf8( ).data( ) );
        return;
    }

    _modelSelection.estimatedVolume = 0.0;
    m->bounds( _modelSelection.vertexCount, _modelSelection.x, _modelSelection.y, _modelSelection.z );

    debug(
        "+ FileTab::loader_gotMesh:\n"
        "  + file name:         '%s'\n"
        "  + count of vertices: %5zu\n"
        "  + X range:           %12.6f .. %12.6f, %12.6f\n"
        "  + Y range:           %12.6f .. %12.6f, %12.6f\n"
        "  + Z range:           %12.6f .. %12.6f, %12.6f\n"
        "",
        _modelSelection.fileName.toUtf8( ).data( ),
        _modelSelection.vertexCount,
        _modelSelection.x.min, _modelSelection.x.max, _modelSelection.x.size,
        _modelSelection.y.min, _modelSelection.y.max, _modelSelection.y.size,
        _modelSelection.z.min, _modelSelection.z.max, _modelSelection.z.size
    );

    {
        auto sizeXstring = GroupDigits( QString( "%1" ).arg( _modelSelection.x.size, 0, 'f', 2 ), ' ' );
        auto sizeYstring = GroupDigits( QString( "%1" ).arg( _modelSelection.y.size, 0, 'f', 2 ), ' ' );
        auto sizeZstring = GroupDigits( QString( "%1" ).arg( _modelSelection.z.size, 0, 'f', 2 ), ' ' );
        _dimensionsLabel->setText( QString( "%1 mm × %2 mm × %3 mm" ).arg( sizeXstring ).arg( sizeYstring ).arg( sizeZstring ) );
    }

    _canvas->load_mesh( m );

    if ( ( _modelSelection.x.size > PrinterMaximumX ) || ( _modelSelection.y.size > PrinterMaximumY ) || ( _modelSelection.z.size > PrinterMaximumZ ) ) {
        _dimensionsErrorLabel->show( );
        _selectButton->setEnabled( false );
        emit modelSelectionFailed( );
        return;
    } else {
        _dimensionsErrorLabel->hide( );
    }

    if ( _processRunner ) {
        QObject::disconnect( _processRunner, nullptr, this, nullptr );
        _processRunner->terminate( );
        _processRunner->deleteLater( );
        _processRunner = nullptr;
    }

    _processRunner = new ProcessRunner( this );
    QObject::connect( _processRunner, &ProcessRunner::succeeded,               this, &FileTab::processRunner_succeeded               );
    QObject::connect( _processRunner, &ProcessRunner::failed,                  this, &FileTab::processRunner_failed                  );
    QObject::connect( _processRunner, &ProcessRunner::readyReadStandardOutput, this, &FileTab::processRunner_readyReadStandardOutput );
    QObject::connect( _processRunner, &ProcessRunner::readyReadStandardError,  this, &FileTab::processRunner_readyReadStandardError  );

    _processRunner->start(
        { "slic3r" },
        {
            { "--info"                 },
            { _modelSelection.fileName }
        }
    );
}

void FileTab::loader_ErrorBadStl( ) {
    debug( "+ FileTab::loader_ErrorBadStl\n" );
    QMessageBox::critical( this, "Error",
        "<b>Error:</b><br>"
        "This <code>.stl</code> file is invalid or corrupted.<br>"
        "Please export it from the original source, verify, and retry."
    );

    _selectButton->setEnabled( false );
    emit modelSelectionFailed( );
}

void FileTab::loader_ErrorEmptyMesh( ) {
    debug( "+ FileTab::loader_ErrorEmptyMesh\n" );
    QMessageBox::critical( this, "Error",
        "<b>Error:</b><br>"
        "This file is syntactically correct<br>"
        "but contains no triangles."
    );

    _selectButton->setEnabled( false );
    emit modelSelectionFailed( );
}

void FileTab::loader_ErrorMissingFile( ) {
    debug( "+ FileTab::loader_ErrorMissingFile\n" );
    QMessageBox::critical( this, "Error",
        "<b>Error:</b><br>"
        "The target file is missing."
    );

    _selectButton->setEnabled( false );
    emit modelSelectionFailed( );
}

void FileTab::loader_Finished( ) {
    debug( "+ FileTab::loader_Finished\n" );
    _availableFilesListView->setEnabled( true );
    _canvas->clear_status( );
    _loader->deleteLater( );
    _loader = nullptr;
}

void FileTab::processRunner_succeeded( ) {
    debug( "+ FileTab::processRunner_succeeded\n" );

    bool gotVolume = false;
    for ( auto line : _slicerBuffer.split( QRegularExpression { QString { "\\r?\\n" } } ) ) {
        auto match = VolumeLineMatcher.match( line );
        if ( match.hasMatch( ) ) {
            _modelSelection.estimatedVolume = match.captured( 1 ).toDouble( );
            if ( _modelSelection.estimatedVolume < 1000.0 ) {
                _dimensionsLabel->setText( _dimensionsLabel->text( ) + QString( "  •  %1 µL" ).arg( GroupDigits( QString( "%1" ).arg( _modelSelection.estimatedVolume,          0, 'f', 2 ), ' ' ) ) );
            } else {
                _dimensionsLabel->setText( _dimensionsLabel->text( ) + QString( "  •  %1 mL" ).arg( GroupDigits( QString( "%1" ).arg( _modelSelection.estimatedVolume / 1000.0, 0, 'f', 2 ), ' ' ) ) );
            }
            gotVolume = true;
            break;
        }
    }

    if ( gotVolume ) {
        _selectButton->setEnabled( true );
    }

    _slicerBuffer.clear( );
}

void FileTab::processRunner_failed( QProcess::ProcessError const error ) {
    debug( "FileTab::processRunner_failed: error %s [%d]\n", ToString( error ), error );
    _slicerBuffer.clear( );
    emit modelSelectionFailed( );
}

void FileTab::processRunner_readyReadStandardOutput( QString const& data ) {
    debug( "+ FileTab::processRunner_readyReadStandardOutput: %d bytes from slic3r\n", data.length( ) );
    _slicerBuffer += data;
}

void FileTab::processRunner_readyReadStandardError( QString const& data ) {
    debug(
        "+ FileTab::processRunner_readyReadStandardError: %d bytes from slic3r:\n"
        "%s",
        data.length( ),
        data.toUtf8( ).data( )
    );
}

bool FileTab::_loadModel( QString const& fileName ) {
    debug( "+ FileTab::_loadModel: fileName: '%s'\n", fileName.toUtf8( ).data( ) );
    if ( _loader ) {
        debug( "+ FileTab::_loadModel: loader object exists, not loading\n" );
        return false;
    }

    _canvas->set_status( QString( "Loading " ) + GetFileBaseName( fileName ) );

    _loader = new Loader( fileName, this );
    connect( _loader, &Loader::got_mesh,           this,    &FileTab::loader_gotMesh          );
    connect( _loader, &Loader::error_bad_stl,      this,    &FileTab::loader_ErrorBadStl      );
    connect( _loader, &Loader::error_empty_mesh,   this,    &FileTab::loader_ErrorEmptyMesh   );
    connect( _loader, &Loader::error_missing_file, this,    &FileTab::loader_ErrorMissingFile );
    connect( _loader, &Loader::finished,           this,    &FileTab::loader_Finished         );

    _selectButton->setEnabled( false );
    _loader->start( );
    return true;
}

void FileTab::_showLibrary( ) {
    _modelsLocation = ModelsLocation::Library;
    _currentFsModel = _libraryFsModel;

    _libraryFsModel->sort( 0, Qt::AscendingOrder );
    _availableFilesLabel->setText( "Models in library:" );
    _availableFilesListView->setModel( _libraryFsModel );
    _availableFilesListView->setRootIndex( _libraryFsModel->index( StlModelLibraryPath ) );
    _toggleLocationButton->setText( "Show USB stick" );
}

void FileTab::_showUsbStick( ) {
    _modelsLocation = ModelsLocation::Usb;
    _currentFsModel = _usbFsModel;

    _usbFsModel->sort( 0, Qt::AscendingOrder );
    _availableFilesLabel->setText( "Models on USB stick:" );
    _availableFilesListView->setModel( _usbFsModel );
    _availableFilesListView->setRootIndex( _usbFsModel->index( _usbPath ) );
    _toggleLocationButton->setText( "Show library" );
}