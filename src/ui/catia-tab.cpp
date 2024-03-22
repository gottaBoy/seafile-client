#include <cstdio>
#include <QtGlobal>

#include <QtWidgets>
#include <QIcon>
#include <QStackedWidget>
#include <QModelIndex>
#include <QLabel>

#include "seafile-applet.h"
#include "account-mgr.h"
#include "catia-events-list-view.h"
#include "loading-view.h"
#include "logout-view.h"
//#include "events-service.h"
#include "catia-events-service.h"
#include "avatar-service.h"
#include "api/api-error.h"
#include "utils/utils.h"

#include "catia-tab.h"
#include "ui/catia-search-bar.h"

namespace {

//const int kRefreshInterval = 1000 * 60 * 5; // 5 min
const char *kLoadingFailedLabelName = "loadingFailedText";
//const char *kEmptyViewLabelName = "emptyText";
//const char *kAuthHeader = "Authorization";
//const char *kCatiaUrl = "/api2/html/events/";

enum {
    INDEX_LOADING_VIEW = 0,
    INDEX_LOADING_FAILED_VIEW,
    INDEX_LOGOUT_VIEW,
    INDEX_EVENTS_VIEW,
};


}


CatiaTab::CatiaTab(QWidget *parent)
    : TabView(parent)
{
    createEventsView();
    createLoadingView();
    createLoadingFailedView();

    //createLogoutView
    logout_view_ = new LogoutView;
    static_cast<LogoutView*>(logout_view_)->setQssStyleForTab();

    catia_filter_text_ = new CatiaSearchBar;
    catia_filter_text_->setPlaceholderText(tr("Search Catia"));
    connect(catia_filter_text_, SIGNAL(textChanged(const QString&)),
            this, SLOT(onFilterTextChanged(const QString&)));

    QVBoxLayout *vlayout = (QVBoxLayout *)layout();
    vlayout->setSpacing(0);
    vlayout->insertWidget(0, catia_filter_text_);

    mStack->insertWidget(INDEX_LOADING_VIEW, loading_view_);
    mStack->insertWidget(INDEX_LOADING_FAILED_VIEW, loading_failed_view_);
    mStack->insertWidget(INDEX_LOGOUT_VIEW, logout_view_);
    mStack->insertWidget(INDEX_EVENTS_VIEW, catia_events_container_view_);

    connect(CatiaEventsService::instance(), SIGNAL(refreshSuccess(const std::vector<CatiaEvent>&, bool, bool)),
            this, SLOT(refreshEvents(const std::vector<CatiaEvent>&, bool, bool)));
    connect(CatiaEventsService::instance(), SIGNAL(refreshFailed(const ApiError&)),
            this, SLOT(refreshFailed(const ApiError&)));

    connect(AvatarService::instance(), SIGNAL(avatarUpdated(const QString&, const QImage&)),
            catia_events_list_model_, SLOT(onAvatarUpdated(const QString&, const QImage&)));

    refresh();
}

void CatiaTab::showEvent(QShowEvent *event)
{
    TabView::showEvent(event);
    if (mStack->currentIndex() == INDEX_EVENTS_VIEW) {
        catia_events_list_view_->update();
    }
}

void CatiaTab::loadMoreEvents()
{
    CatiaEventsService::instance()->loadMore();
}

void CatiaTab::refreshEvents(const std::vector<CatiaEvent>& events,
                                  bool is_loading_more,
                                  bool has_more)
{
    catia_events_list_model_->removeRow(
        catia_events_list_model_->loadMoreIndex().row());

    mStack->setCurrentIndex(INDEX_EVENTS_VIEW);

    // XXX: "load more events" for now
    const QModelIndex first =
        catia_events_list_model_->updateEvents(
        (const std::vector<CatiaEvent> &)events, is_loading_more, has_more);
    if (first.isValid()) {
        catia_events_list_view_->scrollTo(first);
    }

    if (has_more) {
        load_more_btn_ = new LoadMoreButton;
        connect(load_more_btn_, SIGNAL(clicked()),
                this, SLOT(loadMoreEvents()));
        catia_events_list_view_->setIndexWidget(
            catia_events_list_model_->loadMoreIndex(), load_more_btn_);
    }
}

void CatiaTab::refresh()
{
    if (!seafApplet->accountManager()->hasAccount() ||
        !seafApplet->accountManager()->accounts().front().isValid()) {
        mStack->setCurrentIndex(INDEX_LOGOUT_VIEW);
        return;
    }
    showLoadingView();

    CatiaEventsService::instance()->refresh(true);
}

void CatiaTab::createEventsView()
{
    catia_events_container_view_ = new QWidget;
    catia_events_container_view_->setObjectName("CatiaEventsContainerView");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    catia_events_container_view_->setLayout(layout);

    catia_events_list_view_ = new CatiaEventsListView;
    layout->addWidget(catia_events_list_view_);

    catia_events_list_model_ = new CatiaEventsListModel;
    catia_events_list_view_->setModel(catia_events_list_model_);
}

void CatiaTab::createLoadingView()
{
    loading_view_ = new LoadingView;
    static_cast<LoadingView*>(loading_view_)->setQssStyleForTab();
}

void CatiaTab::createLoadingFailedView()
{
    loading_failed_view_ = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout;
    loading_failed_view_->setLayout(layout);

    loading_failed_text_ = new QLabel;
    loading_failed_text_->setObjectName(kLoadingFailedLabelName);
    loading_failed_text_->setAlignment(Qt::AlignCenter);

    connect(loading_failed_text_, SIGNAL(linkActivated(const QString&)),
            this, SLOT(refresh()));

    layout->addWidget(loading_failed_text_);
}

void CatiaTab::showLoadingView()
{
    mStack->setCurrentIndex(INDEX_LOADING_VIEW);
}

void CatiaTab::refreshFailed(const ApiError& error)
{
    QString text;
    if (error.type() == ApiError::HTTP_ERROR
        && error.httpErrorCode() == 404) {
        text = tr("File Catia are only supported in %1 Server Professional Edition.").arg(getBrand());
    } else {
        QString link = QString("<a style=\"color:#777\" href=\"#\">%1</a>").arg(tr("retry"));
        text = tr("Failed to get catia information. "
                  "Please %1").arg(link);
    }

    loading_failed_text_->setText(text);

    mStack->setCurrentIndex(INDEX_LOADING_FAILED_VIEW);
}

void CatiaTab::startRefresh()
{
    AccountManager *mgr = seafApplet->accountManager();
    bool has_pro_account = mgr->hasAccount() && mgr->accounts().front().isPro();
    if (has_pro_account)
        CatiaEventsService::instance()->start();
}

void CatiaTab::stopRefresh()
{
    CatiaEventsService::instance()->stop();
}

void CatiaTab::onFilterTextChanged(const QString& text)
{
//    catia_events_list_model_->onFilterTextChanged(text);
//    filter_model_->setFilterText(text.trimmed());
//    filter_model_->sort(0);
//    if (text.isEmpty()) {
//        repos_tree_->restoreExpandedCategries();
//    } else {
//        repos_tree_->expandAll();
//    }
}
