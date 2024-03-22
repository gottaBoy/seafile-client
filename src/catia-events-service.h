#ifndef SEAFILE_CLIENT_CATIA_EVENTS_SERVICE_H
#define SEAFILE_CLIENT_CATIA_EVENTS_SERVICE_H

#include <vector>
#include <QObject>

//#include "api/event.h"
#include "api/catia-event.h"

class ApiError;
class GetCatiaEventsRequest;

class CatiaEventsService : public QObject
{
    Q_OBJECT
public:
    static CatiaEventsService* instance();

    void start();
    void stop();

    void refresh(bool force);

    void loadMore();

    // accessors
    const std::vector<CatiaEvent>& events() const { return events_; }

    bool hasMore() const { return next_ > 0; }

public slots:
    void refresh();

private slots:
    void onRefreshSuccess(const std::vector<CatiaEvent>& events, int more_offset);
    void onRefreshSuccessV2(const std::vector<CatiaEvent>& events);
    void onRefreshFailed(const ApiError& error);

signals:
    void refreshSuccess(const std::vector<CatiaEvent>& events, bool is_loading_more, bool has_more);
    void refreshFailed(const ApiError& error);

private:
    Q_DISABLE_COPY(CatiaEventsService)

    CatiaEventsService(QObject *parent=0);
    void sendRequest(bool is_loading_more);

    static CatiaEventsService *singleton_;

    const std::vector<CatiaEvent> handleEventsOffset(const std::vector<CatiaEvent>& new_events);

    GetCatiaEventsRequest *get_catia_events_req_;

    GetCatiaEventsRequest *get_file_catia_req_;

    std::vector<CatiaEvent> events_;

    bool in_refresh_;

    // for old api, it's an offset
    // for new api, it's the next page number
    int next_;
};


#endif // SEAFILE_CLIENT_CATIA_EVENTS_SERVICE_H
