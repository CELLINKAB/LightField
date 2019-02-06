#ifndef __PNGDISPLAYER_H__
#define __PNGDISPLAYER_H__

#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QString>
#include <QWidget>

class PngDisplayer: public QMainWindow {

    Q_OBJECT

public:

    PngDisplayer( QWidget* parent = nullptr );
    virtual ~PngDisplayer( ) override;

    bool load( QString const& fileName );

protected:

private:

    QLabel* _label;
    QPixmap* _png { new QPixmap };

signals:

public slots:

protected slots:

private slots:

};

#endif // __PNGDISPLAYER_H__
