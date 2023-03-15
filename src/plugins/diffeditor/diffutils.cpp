// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0
#include <QFutureInterfaceBase>
static QString leftFileName(const FileData &fileData, unsigned formatFlags)
    QString diffText;
    QTextStream str(&diffText);
    if (fileData.fileOperation == FileData::NewFile) {
        str << "/dev/null";
    } else {
        if (formatFlags & DiffUtils::AddLevel)
            str << "a/";
        str << fileData.fileInfo[LeftSide].fileName;
    }
    return diffText;
}

static QString rightFileName(const FileData &fileData, unsigned formatFlags)
{
    QString diffText;
    QTextStream str(&diffText);
    if (fileData.fileOperation == FileData::DeleteFile) {
        str << "/dev/null";
    } else {
        if (formatFlags & DiffUtils::AddLevel)
            str << "b/";
        str << fileData.fileInfo[RightSide].fileName;
    }
    return diffText;
QString DiffUtils::makePatch(const QList<FileData> &fileDataList, unsigned formatFlags)
        if (formatFlags & GitFormat) {
            str << "diff --git a/" << fileData.fileInfo[LeftSide].fileName
                << " b/" << fileData.fileInfo[RightSide].fileName << '\n';
        }
            str << leftFileName(fileData, formatFlags);
            str << rightFileName(fileData, formatFlags);
                str << leftFileName(fileData, formatFlags) << "\n";
                str << rightFileName(fileData, formatFlags) << "\n";
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
    const QString devNull("/dev/null");
    const QString leftFileName = fileData->fileOperation == FileData::NewFile
            ? devNull : QLatin1String("a/") + fileData->fileInfo[LeftSide].fileName;
    const QString rightFileName = fileData->fileOperation == FileData::DeleteFile
            ? devNull : QLatin1String("b/") + fileData->fileInfo[RightSide].fileName;

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
}

int DiffUtils::interpolate(int x, int x1, int x2, int y1, int y2)
{
    if (x1 == x2)
        return x1;
    if (x == x1)
        return y1;
    if (x == x2)
        return y2;
    const int numerator = (y2 - y1) * x + x2 * y1 - x1 * y2;
    const int denominator = x2 - x1;
    return qRound((double)numerator / denominator);