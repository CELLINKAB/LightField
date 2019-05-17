#include "pch.h"

#include "svgrenderer.h"

#include "processrunner.h"
#include "strings.h"

namespace {

    QDomDocument _CloneDocRoot( QDomDocument& doc ) {
        QDomDocument skeletonDoc( doc.doctype( ) );
        skeletonDoc.appendChild( doc.childNodes( ).item( 0 ).cloneNode( false ) );
        skeletonDoc.appendChild( doc.childNodes( ).item( 1 ).cloneNode( false ) );
        return skeletonDoc;
    }

}

SvgRenderer::SvgRenderer( ) {
    _processRunner = new ProcessRunner;
    QObject::connect( _processRunner, &ProcessRunner::succeeded, this, &SvgRenderer::programSucceeded );
    QObject::connect( _processRunner, &ProcessRunner::failed,    this, &SvgRenderer::programFailed    );
}

SvgRenderer::~SvgRenderer( ) {
    /*empty*/
}

void SvgRenderer::startRender( QString const& svgFileName, QString const& outputDirectory ) {
    debug( "+ SvgRenderer::startRender\n" );
    _outputDirectory = outputDirectory;

    QFile file { svgFileName };
    if ( !file.open( QIODevice::ReadOnly ) ) {
        debug( "  + couldn't open file '%s'", svgFileName.toUtf8( ).data( ) );
        emit done( false );
        return;
    }
    if ( !_doc.setContent( &file ) ) {
        debug( "  + couldn't load file\n" );
        file.close( );
        emit done( false );
        return;
    }
    file.close( );

    QDomDocument skeletonDoc = _CloneDocRoot( _doc );

    QDomElement svgElement = _doc.documentElement( );
    if ( !svgElement.hasAttributes( ) ) {
        debug( "  + SVG element has no attributes?\n" );
        emit done( false );
        return;
    }

    auto mmWidth  = svgElement.attribute( "width" ).toDouble( );
    auto mmHeight = svgElement.attribute( "height" ).toDouble( );
    _pxWidth  = mmWidth  * 20.0 + 0.5;
    _pxHeight = mmHeight * 20.0 + 0.5;
    debug( "  + dimensions: mm %f×%f; px %d×%d\n", mmWidth, mmHeight, _pxWidth, _pxHeight );

    QDomNodeList childNodes = svgElement.childNodes( );
    int limit = childNodes.length( );
    debug( "  + childNodes.length(): %d\n", childNodes.length( ) );

    _currentLayer = 0;
    for ( int childIndex = 0; childIndex < limit; ++childIndex ) {
        QDomNode node = childNodes.item( childIndex );
        if ( !node.isElement( ) ) {
            debug( "  + skipping non-element node\n" );
            continue;
        }

        QDomElement* element = reinterpret_cast<QDomElement*>( &node );
        QString nodeName = element->nodeName( );
        if ( nodeName != "g" ) {
            debug( "  + skipping element because it's not <g>\n" );
            continue;
        }
        if ( !element->hasAttributes( ) ) {
            debug( "  + skipping element because it has no attributes\n" );
            continue;
        }

        QString idValue = element->attribute( "id" );
        if ( !idValue.startsWith( "layer" ) ) {
            debug( "  + skipping element because its id attribute does not start with 'layer'\n" );
            continue;
        }

        QDomDocument layerDoc = _CloneDocRoot( skeletonDoc );
        auto docElt = layerDoc.documentElement( );
        docElt.appendChild( element->cloneNode( true ) );
        docElt.setAttribute( "width",  QString( "%1" ).arg( mmWidth,  0, 'f', 2 ) );
        docElt.setAttribute( "height", QString( "%1" ).arg( mmHeight, 0, 'f', 2 ) );

        QFile data( QString( "%1/%2.svg" ).arg( _outputDirectory ).arg( _currentLayer++, 6, 10, DigitZero ) );
        if ( data.open( QFile::WriteOnly | QFile::Truncate ) ) {
            QTextStream textStream( &data );
            layerDoc.save( textStream, 0 );
            data.close( );
        } else {
            debug( "+ SvgRenderer::startRender: save failed\n" );
            emit done( false );
            return;
        }
    }
    _totalLayers = _currentLayer;
    emit layerCount( _totalLayers );

    _currentLayer = 0;
    _renderLayer( );
}

void SvgRenderer::_renderLayer( ) {
    debug( "+ SvgRenderer::_renderLayer: _currentLayer: %d; PNG size: %d×%d\n", _currentLayer, _pxWidth, _pxHeight );
    _processRunner->setProcessChannelMode( QProcess::ForwardedChannels );
    _processRunner->start(
        { "gm" },
        {
            "convert",
            "-antialias",
            "-density",    "400",
            "-background", "#000000",
            "-size",       QString( "%1x%2" ).arg( _pxWidth ).arg( _pxHeight ),
            QString( "%1/%2.svg" ).arg( _outputDirectory ).arg( _currentLayer, 6, 10, DigitZero ),
            QString( "%1/%2.png" ).arg( _outputDirectory ).arg( _currentLayer, 6, 10, DigitZero )
        }
    );
}

void SvgRenderer::programSucceeded( ) {
    emit layerComplete( _currentLayer );

    if ( _currentLayer + 1 == _totalLayers ) {
        debug( "+ SvgRenderer::programSucceeded: finished\n" );
        emit done( true );
    } else {
        ++_currentLayer;
        _renderLayer( );
    }
}

void SvgRenderer::programFailed( QProcess::ProcessError const error ) {
    debug( "+ SvgRenderer::programFailed: error: %s [%d]\n", ToString( error ), static_cast<int>( error ) );
    emit done( false );
}
