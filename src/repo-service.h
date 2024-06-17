#ifndef SEAFILE_CLIENT_REPO_SERVICE_H_
#define SEAFILE_CLIENT_REPO_SERVICE_H_

#include <vector>
#include <QObject>
#include <QRunnable>

#include "utils/singleton.h"
#include "rpc/local-repo.h"
#include "api/server-repo.h"
#include "filebrowser/seaf-dirent.h"

class QTimer;

class ApiError;
class ListReposRequest;
class GetRepoRequest;
class ListPdmReposRequest;
class ListEbomReposRequest;
class SeafDirent;
struct sqlite3;


class RepoService : public QObject {
    SINGLETON_DEFINE(RepoService)
    Q_OBJECT
public:
    void start();
    void stop();

    const std::vector<ServerRepo>& serverRepos() const { return server_repos_; }

    const std::vector<LocalRepo>& localRepos() const { return local_repos_; }

    ServerRepo getRepo(const QString& repo_id) const;
    ServerRepo getPdmRepo() const;
    ServerRepo getMyRepo() const;
    QList<SeafDirent> getEbomRepos() const;

    void saveSyncedSubfolder(const ServerRepo& subfolder);
    void removeSyncedSubfolder(const QString& repo_id);

    void refreshLocalRepoList();

    void refresh(bool force);

    void openLocalFile(const QString& repo_id,
                       const QString& path_in_repo,
                       QWidget *dialog_parent=0);

    void openRepoOnSeahub(const QString& repo_id);

    void openFolder(const QString &repo_id,
                    const QString &path_in_repo);
    void removeCloudFileBrowserCache();
public slots:
    void refresh();
    void getPdm();
    void getEbom();

private slots:
    void onListPdmSuccess(const std::vector<ServerRepo>& repos);
    void onListPdmFailed(const ApiError& error);
    void onListEbomSuccess(const QList<SeafDirent>&);
    void onListEbomFailed(const ApiError& error);
    void onRefreshSuccess(const std::vector<ServerRepo>& repos);
    void onRefreshFailed(const ApiError& error);

    void onGetRequestSuccess(const ServerRepo& repo);
    void onGetRequestFailed(const ApiError& error);

    void onWiperDone();

    void onRemoteWipeReportSuccess();
    void onRemoteWipeReportFailed(const ApiError& error);

signals:
    void refreshSuccess(const std::vector<ServerRepo>& repos);
    void refreshFailed(const ApiError& error);
    void getPdmSuccess();

private:
    Q_DISABLE_COPY(RepoService)
    ~RepoService();

    RepoService(QObject *parent=0);

    void startGetRequestFor(const QString &repo_id);
    void wipeLocalFiles();
    bool isGrepos(const ServerRepo& repo);

    ListReposRequest *list_repo_req_;
    ListPdmReposRequest *list_pdm_repo_req_;
    ListEbomReposRequest *list_ebom_repo_req_;
    std::list<GetRepoRequest*> get_repo_reqs_;
    struct sqlite3 *synced_subfolder_db_;

    std::vector<ServerRepo> server_repos_;
    std::vector<LocalRepo> local_repos_;

    QTimer *refresh_timer_;
    bool in_refresh_;
    bool has_name_repo_;

    bool wipe_in_progress_;
    std::vector<ServerRepo> pdm_repos_;
    std::vector<ServerRepo> my_repos_;
    QList<SeafDirent> ebom_dirents_;
};


class WipeFilesThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    WipeFilesThread(const std::vector<LocalRepo>& local_repos,
                    const QStringList& cached_files)
        : local_repos_(local_repos),
          cached_files_(cached_files) {};

    void run();

signals:
    void done();

private:
    const std::vector<LocalRepo> local_repos_;
    const QStringList cached_files_;
};

#endif // SEAFILE_CLIENT_REPO_SERVICE_H_
