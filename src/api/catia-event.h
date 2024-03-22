#ifndef SEAFILE_CLIENT_API_EVENT_CATIA_H
#define SEAFILE_CLIENT_API_EVENT_CATIA_H

#include <jansson.h>
#include <vector>
#include "event.h"

#include <QObject>
#include <QString>
#include <QMetaType>

class CatiaEvent : public SeafEvent {
public:
    QString author;
    QString nick;
    QString repo_id;
    QString repo_name;
    QString etype;
    QString commit_id;
    QString desc;
    QString op_desc;
    qint64 timestamp;
    bool is_use_new_activities_api;
    QString biz_id;
    QString biz_version;
    QString biz_name;
    qint64 update_time;

    // true for events like a file upload by unregistered user from a
    // uploadable link
    bool anonymous;

    bool isDetailsDisplayable() const;
    
    static CatiaEvent fromJSON(const json_t*, json_error_t *error);
    static CatiaEvent fromJSONV2(const json_t*, json_error_t *error);
    static std::vector<CatiaEvent> listFromJSON(const json_t*, json_error_t *json, bool is_use_new_json_parser = false);

    QString toString() const;
};

/**
 * Register with QMetaType so we can wrap it with QVariant::fromValue
 */
Q_DECLARE_METATYPE(CatiaEvent)

#endif // SEAFILE_CLIENT_API_EVENT_CATIA_H
