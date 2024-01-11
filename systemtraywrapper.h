#ifndef SYSTEMTRAYWRAPPER_H
#define SYSTEMTRAYWRAPPER_H

#include <QWidget>
#include <QSystemTrayIcon>

// Fwd class decl.
class QMenu;

class SystemTrayWrapper : public QWidget
{
    Q_OBJECT

public:
    explicit SystemTrayWrapper(QWidget *parent = nullptr);
    ~SystemTrayWrapper();

    void setVisible(bool visible) override;
    void setIconFile (const QString &f);

    void setSysTrayedChildWidget(QWidget *w) { _sysTrayedChildWidget = w; }
    const QWidget* sysTrayedChildWidget()  { return _sysTrayedChildWidget; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showSearchDialog();
    void showSettingsDialog();

signals:

private:
    void createActions();
    void createTrayIcon();


private:
    QAction *           minimizeAction;
    QAction *           maximizeAction;
    QAction *           restoreAction;
    QAction *           quitAction;

    QSystemTrayIcon *   trayIcon;
    QMenu *             trayIconMenu;
    QWidget *           _sysTrayedChildWidget;

};

#endif // SYSTEMTRAYWRAPPER_H
