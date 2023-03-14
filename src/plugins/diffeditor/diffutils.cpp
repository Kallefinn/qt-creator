// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
#include <QFuture>
#include <QPromise>
static QString sideFileName(DiffSide side, const FileData &fileData)
    const FileData::FileOperation operation = side == LeftSide ? FileData::NewFile
                                                               : FileData::DeleteFile;
    if (fileData.fileOperation == operation)
        return "/dev/null";
    const QString sideMarker = side == LeftSide ? QString("a/") : QString("b/");
    return sideMarker + fileData.fileInfo[side].fileName;
QString DiffUtils::makePatch(const QList<FileData> &fileDataList)
        str << "diff --git a/" << fileData.fileInfo[LeftSide].fileName
                      << " b/" << fileData.fileInfo[RightSide].fileName << '\n';
            str << sideFileName(LeftSide, fileData);
            str << sideFileName(RightSide, fileData);
                str << sideFileName(LeftSide, fileData) << "\n";
                str << sideFileName(RightSide, fileData) << "\n";
    const QList<QStringView> lines = patch.split(newLine);
    QList<int> startingPositions; // store starting positions of @@
static void readDiffPatch(QPromise<QList<FileData>> &promise, QStringView patch)
            if (promise.isCanceled())
                return;
                const FileData fileData = readDiffHeaderAndChunks(headerAndChunks, &readOk);
            const FileData fileData = readDiffHeaderAndChunks(headerAndChunks, &readOk);
        return;
    promise.addResult(fileDataList);
    const QString leftFileName = sideFileName(LeftSide, *fileData);
    const QString rightFileName = sideFileName(RightSide, *fileData);
static void readGitPatch(QPromise<QList<FileData>> &promise, QStringView patch)
    QList<int> startingPositions; // store starting positions of git headers
    QList<PatchInfo> patches;
        if (promise.isCanceled())
            return;
        if (!detectFileData(fileDiff, &fileData, &remainingFileDiff))
            return;
    if (patches.isEmpty())
        return;
    promise.setProgressRange(0, patches.size());
        if (promise.isCanceled())
            return;
        promise.setProgressValue(i++);
        bool readOk = false;
            return;
    promise.addResult(fileDataList);
}
std::optional<QList<FileData>> DiffUtils::readPatch(const QString &patch)
{
    QPromise<QList<FileData>> promise;
    promise.start();
    readPatchWithPromise(promise, patch);
    if (promise.future().resultCount() == 0)
    return promise.future().result();
void DiffUtils::readPatchWithPromise(QPromise<QList<FileData>> &promise, const QString &patch)
    promise.setProgressRange(0, 1);
    promise.setProgressValue(0);
    readGitPatch(promise, croppedPatch);
    if (promise.future().resultCount() == 0)
        readDiffPatch(promise, croppedPatch);