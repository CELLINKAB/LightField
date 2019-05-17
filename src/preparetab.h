#ifndef __PREPARETAB_H__
#define __PREPARETAB_H__

#include "tabbase.h"

class Hasher;
class SvgRenderer;

class PrepareTab: public InitialShowEventMixin<PrepareTab, TabBase> {

    Q_OBJECT

public:

    PrepareTab( QWidget* parent = nullptr );
    virtual ~PrepareTab( ) override;

    bool             isPrepareButtonEnabled( ) const          { return _prepareButton->isEnabled( ); }
    bool             isSliceButtonEnabled( )   const          { return _sliceButton->isEnabled( );   }

    virtual TabIndex tabIndex( )               const override { return TabIndex::Prepare;            }

protected:

    virtual void _connectShepherd( )                   override;
    virtual void initialShowEvent( QShowEvent* event ) override;

private:

    QProcess*     _slicerProcess               { };
    SvgRenderer*  _svgRenderer                 { };
    Hasher*       _hasher                      { };
    int           _visibleLayer                { };
    int           _renderedLayers              { };
    bool          _isPrinterOnline             { false };
    bool          _isPrinterAvailable          { true  };

    QLabel*       _layerThicknessLabel         { new QLabel       };
    QRadioButton* _layerThickness50Button      { new QRadioButton };
    QRadioButton* _layerThickness100Button     { new QRadioButton };
    QVBoxLayout*  _layerThicknessButtonsLayout { new QVBoxLayout  };

    QLabel*       _sliceStatusLabel            { new QLabel       };
    QLabel*       _sliceStatus                 { new QLabel       };
    QLabel*       _imageGeneratorStatusLabel   { new QLabel       };
    QLabel*       _imageGeneratorStatus        { new QLabel       };

    QGroupBox*    _prepareGroup                { new QGroupBox    };
    QLabel*       _prepareMessage              { new QLabel       };
    QProgressBar* _prepareProgress             { new QProgressBar };
    QPushButton*  _prepareButton               { new QPushButton  };
    QVBoxLayout*  _prepareLayout               { new QVBoxLayout  };

    QVBoxLayout*  _optionsLayout               { new QVBoxLayout  };
    QWidget*      _optionsContainer            { new QWidget      };
    QPushButton*  _sliceButton                 { new QPushButton  };

    QGroupBox*    _currentLayerGroup           { new QGroupBox    };
    QLabel*       _currentLayerImage           { new QLabel       };
    QVBoxLayout*  _currentLayerLayout          { new QVBoxLayout  };

    QPushButton*  _navigateFirst               { new QPushButton  };
    QPushButton*  _navigatePrevious            { new QPushButton  };
    QLabel*       _navigateCurrentLabel        { new QLabel       };
    QPushButton*  _navigateNext                { new QPushButton  };
    QPushButton*  _navigateLast                { new QPushButton  };
    QHBoxLayout*  _navigationLayout            {                  };

    QGridLayout*  _layout                      { new QGridLayout  };

    bool _checkPreSlicedFiles( );
    bool _checkJobDirectory( );
    void _setNavigationButtonsEnabled( bool const enabled );
    void _showLayerImage( int const layer );
    void _setSliceControlsEnabled( bool const enabled );
    void _updatePrepareButtonState( );

    void _handlePrepareFailed( );

signals:

    void slicingNeeded( bool const needed );

    void preparePrinterStarted( );
    void preparePrinterComplete( bool const success );

    void printerAvailabilityChanged( bool const available );

public slots:

    virtual void tab_uiStateChanged( TabIndex const sender, UiState const state ) override;

    void setPrinterAvailable( bool const value );

protected slots:

private slots:

    void printer_online( );
    void printer_offline( );

    void layerThickness50Button_clicked( bool );
    void layerThickness100Button_clicked( bool );

    void navigateFirst_clicked( bool );
    void navigatePrevious_clicked( bool );
    void navigateNext_clicked( bool );
    void navigateLast_clicked( bool );

    void sliceButton_clicked( bool );

    void hasher_resultReady( QString const hash );

    void slicerProcess_errorOccurred( QProcess::ProcessError error );
    void slicerProcess_started( );
    void slicerProcess_finished( int exitCode, QProcess::ExitStatus exitStatus );

    void svgRenderer_layerCount( int const totalLayers );
    void svgRenderer_layerComplete( int const currentLayer );
    void svgRenderer_done( bool const success );

    void prepareButton_clicked( bool );
    void shepherd_homeComplete( bool const success );
    void adjustBuildPlatform_complete( bool );
    void shepherd_raiseBuildPlatformMoveToComplete( bool const success );

};

#endif // __PREPARETAB_H__
