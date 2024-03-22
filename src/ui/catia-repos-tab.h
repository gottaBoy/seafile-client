#ifndef SEAFILE_CLIENT_UI_CATIA_REPOS_TAB_H
#define SEAFILE_CLIENT_UI_CATIA_REPOS_TAB_H

#include <QLineEdit>

#include "tab-view.h"
#include "api/server-repo.h"

class QTimer;
class QToolButton;

class RepoTreeModel;
class RepoFilterProxyModel;
class RepoTreeView;
class ListReposRequest;
class ApiError;
class SearchBar;

/**
 * The repos list tab
 */
class CatiaReposTab : public TabView {
    Q_OBJECT
public:
    explicit CatiaReposTab(QWidget *parent=0);

    std::vector<QAction*> getToolBarActions();

public slots:
    void getPdm();
    void refresh();

protected:
    void startRefresh();
    void stopRefresh();

private slots:
    void refreshRepos(const std::vector<ServerRepo>& repos);
    void refreshReposFailed(const ApiError& error);
    void onFilterTextChanged(const QString& text);

private:
    void initCatiaReposTab();
    void createRepoTree();
    void createLoadingView();
    void createLoadingFailedView();
    void showLoadingView();

    RepoTreeModel *repos_model_;
    RepoFilterProxyModel *filter_model_;

    RepoTreeView *repos_tree_;
    QWidget *loading_view_;
    QWidget *loading_failed_view_;
    QWidget *logout_view_;

    SearchBar *filter_text_;

    ListReposRequest *list_repo_req_;
};

#endif // SEAFILE_CLIENT_UI_CATIA_REPOS_TAB_H
