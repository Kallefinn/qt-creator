// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "jsonwizardpagefactory_p.h"

#include "jsonfieldpage.h"
#include "jsonfieldpage_p.h"
#include "jsonfilepage.h"
#include "jsonkitspage.h"
#include "jsonprojectpage.h"
#include "jsonsummarypage.h"
#include "jsonwizardfactory.h"
#include "jsonwizardpagefactory.h"
#include "../projectexplorertr.h"

#include <utils/qtcassert.h>
#include <utils/wizardpage.h>

using namespace Utils;

namespace ProjectExplorer::Internal {

// --------------------------------------------------------------------
// FieldPageFactory:
// --------------------------------------------------------------------

class FieldPageFactory final : public JsonWizardPageFactory
{
public:
    FieldPageFactory();

    WizardPage *create(JsonWizard *wizard, Id typeId, const QVariant &data) final;
    bool validateData(Id typeId, const QVariant &data, QString *errorMessage) final;
};

FieldPageFactory::FieldPageFactory()
{
    setTypeIdsSuffix(QLatin1String("Fields"));

    JsonFieldPage::registerFieldFactory(QLatin1String("Label"), []() { return new LabelField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("Spacer"), []() { return new SpacerField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("LineEdit"), []() { return new LineEditField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("TextEdit"), []() { return new TextEditField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("PathChooser"), []() { return new PathChooserField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("CheckBox"), []() { return new CheckBoxField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("ComboBox"), []() { return new ComboBoxField; });
    JsonFieldPage::registerFieldFactory(QLatin1String("IconList"), []() { return new IconListField; });
}

WizardPage *FieldPageFactory::create(JsonWizard *wizard, Id typeId, const QVariant &data)
{
    Q_UNUSED(wizard)

    QTC_ASSERT(canCreate(typeId), return nullptr);

    auto page = new JsonFieldPage(wizard->expander());

    if (!page->setup(data)) {
        delete page;
        return nullptr;
    }

    return page;
}

bool FieldPageFactory::validateData(Id typeId, const QVariant &data, QString *errorMessage)
{
    QTC_ASSERT(canCreate(typeId), return false);

    const QList<QVariant> list = JsonWizardFactory::objectOrList(data, errorMessage);
    if (list.isEmpty()) {
        *errorMessage = Tr::tr("When parsing fields of page \"%1\": %2")
                .arg(typeId.toString()).arg(*errorMessage);
        return false;
    }

    for (const QVariant &v : list) {
        JsonFieldPage::Field *field = JsonFieldPage::Field::parse(v, errorMessage);
        if (!field)
            return false;
        delete field;
    }

    return true;
}

// --------------------------------------------------------------------
// FilePageFactory:
// --------------------------------------------------------------------

class FilePageFactory final : public JsonWizardPageFactory
{
public:
    FilePageFactory();

    WizardPage *create(JsonWizard *wizard, Id typeId, const QVariant &data) override;
    bool validateData(Id typeId, const QVariant &data, QString *errorMessage) override;
};

FilePageFactory::FilePageFactory()
{
    setTypeIdsSuffix(QLatin1String("File"));
}

WizardPage *FilePageFactory::create(JsonWizard *wizard, Id typeId, const QVariant &data)
{
    Q_UNUSED(wizard)
    Q_UNUSED(data)
    QTC_ASSERT(canCreate(typeId), return nullptr);

    return new JsonFilePage;
}

bool FilePageFactory::validateData(Id typeId, const QVariant &data, QString *errorMessage)
{
    QTC_ASSERT(canCreate(typeId), return false);
    if (!data.isNull() && (data.typeId() != QMetaType::QVariantMap || !data.toMap().isEmpty())) {
        *errorMessage = Tr::tr("\"data\" for a \"File\" page needs to be unset or an empty object.");
        return false;
    }

    return true;
}

// --------------------------------------------------------------------
// KitsPageFactory:
// --------------------------------------------------------------------

const char KEY_PROJECT_FILE[] = "projectFilePath";
const char KEY_REQUIRED_FEATURES[] = "requiredFeatures";
const char KEY_PREFERRED_FEATURES[] = "preferredFeatures";

class KitsPageFactory final : public JsonWizardPageFactory
{
public:
    KitsPageFactory();

    WizardPage *create(JsonWizard *wizard, Id typeId, const QVariant &data) override;
    bool validateData(Id typeId, const QVariant &data, QString *errorMessage) override;
};

KitsPageFactory::KitsPageFactory()
{
    setTypeIdsSuffix(QLatin1String("Kits"));
}

WizardPage *KitsPageFactory::create(JsonWizard *wizard, Id typeId, const QVariant &data)
{
    Q_UNUSED(wizard)
    QTC_ASSERT(canCreate(typeId), return nullptr);

    auto page = new JsonKitsPage;
    const QVariantMap dataMap = data.toMap();
    page->setUnexpandedProjectPath(dataMap.value(QLatin1String(KEY_PROJECT_FILE)).toString());
    page->setRequiredFeatures(dataMap.value(QLatin1String(KEY_REQUIRED_FEATURES)));
    page->setPreferredFeatures(dataMap.value(QLatin1String(KEY_PREFERRED_FEATURES)));

    return page;
}

static bool validateFeatureList(const QVariantMap &data, const QByteArray &key, QString *errorMessage)
{
    QString message;
    JsonKitsPage::parseFeatures(data.value(QLatin1String(key)), &message);
    if (!message.isEmpty()) {
        *errorMessage = Tr::tr("Error parsing \"%1\" in \"Kits\" page: %2")
                .arg(QLatin1String(key), message);
        return false;
    }
    return true;
}

bool KitsPageFactory::validateData(Id typeId, const QVariant &data, QString *errorMessage)
{
    QTC_ASSERT(canCreate(typeId), return false);

    if (data.isNull() || data.typeId() != QMetaType::QVariantMap) {
        *errorMessage = Tr::tr("\"data\" must be a JSON object for \"Kits\" pages.");
        return false;
    }

    QVariantMap tmp = data.toMap();
    if (tmp.value(QLatin1String(KEY_PROJECT_FILE)).toString().isEmpty()) {
        *errorMessage = Tr::tr("\"Kits\" page requires a \"%1\" set.")
                .arg(QLatin1String(KEY_PROJECT_FILE));
        return false;
    }

    return validateFeatureList(tmp, KEY_REQUIRED_FEATURES, errorMessage)
            && validateFeatureList(tmp, KEY_PREFERRED_FEATURES, errorMessage);
}

// --------------------------------------------------------------------
// ProjectPageFactory:
// --------------------------------------------------------------------

static const char KEY_PROJECT_NAME_VALIDATOR[] = "projectNameValidator";
static const char KEY_PROJECT_NAME_VALIDATOR_USER_MESSAGE[] = "trProjectNameValidatorUserMessage";

class ProjectPageFactory final : public JsonWizardPageFactory
{
public:
    ProjectPageFactory();

    WizardPage *create(JsonWizard *wizard, Id typeId, const QVariant &data) override;
    bool validateData(Id typeId, const QVariant &data, QString *errorMessage) override;
};

ProjectPageFactory::ProjectPageFactory()
{
    setTypeIdsSuffix(QLatin1String("Project"));
}

WizardPage *ProjectPageFactory::create(JsonWizard *wizard, Id typeId, const QVariant &data)
{
    Q_UNUSED(wizard)
    Q_UNUSED(data)
    QTC_ASSERT(canCreate(typeId), return nullptr);

    auto page = new JsonProjectPage;

    QVariantMap tmp = data.isNull() ? QVariantMap() : data.toMap();
    QString description
            = tmp.value(QLatin1String("trDescription"), QLatin1String("%{trDescription}")).toString();
    page->setDescription(wizard->expander()->expand(description));
    QString projectNameValidator
            = tmp.value(QLatin1String(KEY_PROJECT_NAME_VALIDATOR)).toString();
    QString projectNameValidatorUserMessage
            = JsonWizardFactory::localizedString(tmp.value(QLatin1String(KEY_PROJECT_NAME_VALIDATOR_USER_MESSAGE)));

    if (!projectNameValidator.isEmpty()) {
        QRegularExpression regularExpression(projectNameValidator);
        if (regularExpression.isValid())
            page->setProjectNameRegularExpression(regularExpression, projectNameValidatorUserMessage);
    }

    return page;
}

bool ProjectPageFactory::validateData(Id typeId, const QVariant &data, QString *errorMessage)
{
    Q_UNUSED(errorMessage)

    QTC_ASSERT(canCreate(typeId), return false);
    if (!data.isNull() && data.typeId() != QMetaType::QVariantMap) {
        *errorMessage = Tr::tr("\"data\" must be empty or a JSON object for \"Project\" pages.");
        return false;
    }
    QVariantMap tmp = data.toMap();
    QString projectNameValidator
        = tmp.value(QLatin1String(KEY_PROJECT_NAME_VALIDATOR)).toString();
    if (!projectNameValidator.isNull()) {
        QRegularExpression regularExpression(projectNameValidator);
        if (!regularExpression.isValid()) {
            *errorMessage = Tr::tr(
                "Invalid regular expression \"%1\" in \"%2\". %3").arg(
                projectNameValidator, QLatin1String(KEY_PROJECT_NAME_VALIDATOR), regularExpression.errorString());
            return false;
        }
    }

    return true;
}

// --------------------------------------------------------------------
// SummaryPageFactory:
// --------------------------------------------------------------------

static const char KEY_HIDE_PROJECT_UI[] = "hideProjectUi";

class SummaryPageFactory final : public JsonWizardPageFactory
{
public:
    SummaryPageFactory();

    WizardPage *create(JsonWizard *wizard, Id typeId, const QVariant &data) override;
    bool validateData(Id typeId, const QVariant &data, QString *errorMessage) override;
};

SummaryPageFactory::SummaryPageFactory()
{
    setTypeIdsSuffix(QLatin1String("Summary"));
}

WizardPage *SummaryPageFactory::create(JsonWizard *wizard, Id typeId, const QVariant &data)
{
    Q_UNUSED(wizard)
    Q_UNUSED(data)
    QTC_ASSERT(canCreate(typeId), return nullptr);

    auto page = new JsonSummaryPage;
    QVariant hideProjectUi = data.toMap().value(QLatin1String(KEY_HIDE_PROJECT_UI));
    page->setHideProjectUiValue(hideProjectUi);
    return page;
}

bool SummaryPageFactory::validateData(Id typeId, const QVariant &data, QString *errorMessage)
{
    QTC_ASSERT(canCreate(typeId), return false);
    if (!data.isNull() && (data.typeId() != QMetaType::QVariantMap)) {
        *errorMessage = Tr::tr("\"data\" for a \"Summary\" page can be unset or needs to be an object.");
        return false;
    }

    return true;
}

// Setup

void setupJsonWizardPages()
{
    static FieldPageFactory theFieldPageFactory;
    static FilePageFactory theFilePageFactory;
    static KitsPageFactory theKitsPageFactory;
    static ProjectPageFactory theProjectPageFactory;
    static SummaryPageFactory theSummaryPageFactory;
}

} // ProjectExplorer::Internal
