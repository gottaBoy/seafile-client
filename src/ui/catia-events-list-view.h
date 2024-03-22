#ifndef SEAFILE_CLIENT_UI_CATIA_EVENTS_LIST_VIEW_H
#define SEAFILE_CLIENT_UI_CATIA_EVENTS_LIST_VIEW_H

#include <vector>
#include <QListView>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QModelIndex>

#include "api/catia-event.h"

class QImage;
class QEvent;


enum {
    EVENT_ITEM_TYPE = QStandardItem::UserType,
};

class CatiaEventItem : public QStandardItem {
public:
    CatiaEventItem(const CatiaEvent& event);

    virtual int type() const { return EVENT_ITEM_TYPE; }

    const CatiaEvent& event() const { return event_; }

private:

    CatiaEvent event_;
};

class CatiaEventItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CatiaEventItemDelegate(QObject *parent=0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;

private:
    void paintItem(QPainter *painter,
                   const QStyleOptionViewItem& opt,
                   const CatiaEventItem *item) const;

    QSize sizeHintForItem(const QStyleOptionViewItem &option,
                          const CatiaEventItem *item) const;

    CatiaEventItem* getItem(const QModelIndex &index) const;
};

class CatiaEventsListModel : public QStandardItemModel {
    Q_OBJECT
public:
    CatiaEventsListModel(QObject *parent=0);

    const QModelIndex updateEvents(const std::vector<CatiaEvent>& events,
                                   bool is_loading_more,
                                   bool has_more);
    const QModelIndex loadMoreIndex() const { return load_more_index_; }

public slots:
    void onAvatarUpdated(const QString& email, const QImage& img);

private:
    QModelIndex load_more_index_;
};

class CatiaEventsListView : public QListView {
    Q_OBJECT
public:
    CatiaEventsListView(QWidget *parent=0);

    void updateEvents(const std::vector<CatiaEvent>& events, bool is_loading_more);

    bool viewportEvent(QEvent *event);
                                                                                 
private slots:
    void onItemDoubleClicked(const QModelIndex& index);

private:
    Q_DISABLE_COPY(CatiaEventsListView)

    CatiaEventItem* getItem(const QModelIndex &index) const;
};


#endif // SEAFILE_CLIENT_UI_CATIA_EVENTS_LIST_VIEW_H
