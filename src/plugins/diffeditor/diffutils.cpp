#include <QFutureInterfaceBase>
    const QVector<QStringView> lines = patch.split(newLine);
    QVector<int> startingPositions; // store starting positions of @@
static QList<FileData> readDiffPatch(QStringView patch, bool *ok, QFutureInterfaceBase *jobController)


            if (jobController && jobController->isCanceled())
                return {};
                const FileData fileData = readDiffHeaderAndChunks(headerAndChunks,
                                                                  &readOk);
            const FileData fileData = readDiffHeaderAndChunks(headerAndChunks,
                                                              &readOk);

    if (ok)
        *ok = readOk;

        return {};

    return fileDataList;
static QList<FileData> readGitPatch(QStringView patch, bool *ok, QFutureInterfaceBase *jobController)
    QVector<int> startingPositions; // store starting positions of git headers
    bool readOk = true;
    QVector<PatchInfo> patches;
        if (jobController && jobController->isCanceled())
            return {};
        readOk = detectFileData(fileDiff, &fileData, &remainingFileDiff);

        if (!readOk)
            break;
    if (!readOk) {
        if (ok)
            *ok = readOk;
        return {};
    }
    if (jobController)
        jobController->setProgressRange(0, patches.size());
    readOk = false;
        if (jobController) {
            if (jobController->isCanceled())
                return {};
            jobController->setProgressValue(i++);
        }
            break;
    if (ok)
        *ok = readOk;

    if (!readOk)

    return fileDataList;
QList<FileData> DiffUtils::readPatch(const QString &patch, bool *ok,
                                     QFutureInterfaceBase *jobController)
    bool readOk = false;

    QList<FileData> fileDataList;

    if (jobController) {
        jobController->setProgressRange(0, 1);
        jobController->setProgressValue(0);
    }
    fileDataList = readGitPatch(croppedPatch, &readOk, jobController);
    if (!readOk)
        fileDataList = readDiffPatch(croppedPatch, &readOk, jobController);

    if (ok)
        *ok = readOk;

    return fileDataList;