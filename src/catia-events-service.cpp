#include "seafile-applet.h"
#include "account-mgr.h"
#include "api/requests.h"
#include "catia-events-service.h"

namespace {

const int kEventsPerPageForNewApi = 25;
} // namespace

CatiaEventsService* CatiaEventsService::singleton_;

CatiaEventsService* CatiaEventsService::instance()
{
    if (singleton_ == NULL) {
        static CatiaEventsService instance;
        singleton_ = &instance;
    }

    return singleton_;
}

CatiaEventsService::CatiaEventsService(QObject *parent)
    : QObject(parent)
{
    get_catia_events_req_ = NULL;
    get_file_catia_req_ = NULL;
    next_ = -1;
    in_refresh_ = false;
}

void CatiaEventsService::start()
{
}

void CatiaEventsService::stop()
{
}

void CatiaEventsService::refresh()
{
    if (seafApplet->accountManager()->currentAccount().isPro()) {
        sendRequest(false);
    }
}

void CatiaEventsService::sendRequest(bool is_load_more)
{
    if (in_refresh_) {
        return;
    }

    const Account& account = seafApplet->accountManager()->currentAccount();
    if (!account.isValid()) {
        in_refresh_ = false;
        return;
    }

    // server version begin 7.0.0 support new api
    bool is_support_new_file_activities_api = account.isAtLeastVersion(7, 0, 0);
    in_refresh_ = true;

    if (!is_support_new_file_activities_api) {
        if (get_catia_events_req_) {
            get_catia_events_req_->deleteLater();
        }

        if (!is_load_more) {
            events_.clear();
            next_ = -1;
        }

        // TODO
        get_catia_events_req_ = new GetCatiaEventsRequest(account, next_);

        connect(get_catia_events_req_, SIGNAL(success(const std::vector<CatiaEvent>&, int)),
                this, SLOT(onRefreshSuccess(const std::vector<CatiaEvent>&, int)));

        connect(get_catia_events_req_, SIGNAL(failed(const ApiError&)),
                this, SLOT(onRefreshFailed(const ApiError&)));

        get_catia_events_req_->send();
    } else {
        if (get_file_catia_req_) {
            get_file_catia_req_->deleteLater();
        }

        if (!is_load_more) {
            events_.clear();
            next_ = 1;
        } else {
            ++next_;
        }

        get_file_catia_req_ = new GetCatiaEventsRequest(account, next_);

        connect(get_file_catia_req_, SIGNAL(success(const std::vector<CatiaEvent>&)),
                this, SLOT(onRefreshSuccessV2(const std::vector<CatiaEvent>&)));

        connect(get_file_catia_req_, SIGNAL(failed(const ApiError&)),
                this, SLOT(onRefreshFailed(const ApiError&)));

        get_file_catia_req_->send();
    }

}

void CatiaEventsService::loadMore()
{
    sendRequest(true);
}

void CatiaEventsService::onRefreshSuccess(const std::vector<CatiaEvent>& events, int new_offset)
{
    in_refresh_ = false;

    const std::vector<CatiaEvent> new_events = handleEventsOffset(events);

    bool is_loading_more = next_ > 0;
    bool has_more = new_offset > 0;
    next_ = new_offset;
    emit refreshSuccess(new_events, is_loading_more, has_more);
}

void CatiaEventsService::onRefreshSuccessV2(const std::vector<CatiaEvent>& events)
{
    in_refresh_ = false;

    const std::vector<CatiaEvent> new_events = handleEventsOffset(events);

    bool has_more = events.size() == kEventsPerPageForNewApi;
    bool is_loading_more = next_ > 1;
    if (!has_more) {
        next_ = -1;
    }

    emit refreshSuccess(new_events, is_loading_more, has_more);
}

// We use the "offset" param as the starting point of loading more events, but
// if there are new events on the server, the offset would be inaccurate.
const std::vector<CatiaEvent>
CatiaEventsService::handleEventsOffset(const std::vector<CatiaEvent>& new_events)
{
    if (events_.empty()) {
        events_ = new_events;
        return events_;
    }

    const CatiaEvent& last = events_[events_.size() - 1];

    int i = 0, n = new_events.size();

    for (i = 0; i < n; i++) {
        const CatiaEvent& event = new_events[i];
        if (event.timestamp < last.timestamp) {
            break;
        } else if (event.commit_id == last.commit_id) {
            continue;
        } else {
            continue;
        }
    }

    std::vector<CatiaEvent> ret;

    while (i < n) {
        CatiaEvent event = new_events[i++];
        events_.push_back(event);
        ret.push_back(event);
    }

    return ret;
}

void CatiaEventsService::onRefreshFailed(const ApiError& error)
{
    in_refresh_ = false;

    emit refreshFailed(error);
}

void CatiaEventsService::refresh(bool force)
{
    if (force) {
        in_refresh_ = false;
    }

    refresh();
}
