#ifndef SEAFILE_CLIENT_UI_CATIA_TAB_H
#define SEAFILE_CLIENT_UI_CATIA_TAB_H

#include <vector>
#include <QList>
#include <QSslError>

#include "tab-view.h"
#include "api/catia-event.h"

class QUrl;
class QNetworkRequest;
class QNetworkReply;
class LoadMoreButton;
class QLabel;
class QShowEvent;

class Account;
class ApiError;
class CatiaEventsListView;
class CatiaEventsListModel;
class CatiaSearchBar;

/**
 * The catia tab
 */
class CatiaTab : public TabView {
    Q_OBJECT
public:
    explicit CatiaTab(QWidget *parent = nullptr);

public slots:
    void refresh();

protected:
    void startRefresh();
    void stopRefresh();
    virtual void showEvent(QShowEvent *event);

private slots:
    void refreshEvents(const std::vector<CatiaEvent>& events,
                       bool is_loading_more,
                       bool has_more);
    void refreshFailed(const ApiError& error);
    void loadMoreEvents();
    void onFilterTextChanged(const QString& text);

private:
    void createEventsView();
    void createLoadingView();
    void createLoadingFailedView();
    void showLoadingView();
    void loadPage(const Account& account);

    QWidget *loading_view_;
    QWidget *loading_failed_view_;
    QWidget *logout_view_;

    QWidget *catia_events_container_view_;
    CatiaEventsListView *catia_events_list_view_;
    CatiaEventsListModel *catia_events_list_model_;
    QWidget *events_loading_view_;
    LoadMoreButton *load_more_btn_;

    QLabel *loading_failed_text_;

    CatiaSearchBar *catia_filter_text_;

};

#endif // SEAFILE_CLIENT_UI_CATIA_TAB_H
