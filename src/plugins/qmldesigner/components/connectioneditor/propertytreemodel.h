// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "propertymetainfo.h"
#include <modelnode.h>

#include <studioquickwidget.h>
#include <QAbstractItemModel>
#include <QAbstractListModel>

#include <set>
#include <vector>

namespace QmlDesigner {

class AbstractProperty;
class ModelNode;
class BindingProperty;
class SignalHandlerProperty;
class VariantProperty;

class ConnectionView;

class PropertyTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum UserRoles {
        PropertyNameRole = Qt::UserRole + 1,
        PropertyPriorityRole,
        ExpressionRole,
        RowRole,
        InternalIdRole
    };

    enum PropertyTypes {
        AllTypes,
        NumberType,
        StringType,
        ColorType,
        SignalType,
        SlotType,
        UrlType,
        BoolType
    };

    PropertyTreeModel(ConnectionView *parent = nullptr);

    void resetModel();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    QPersistentModelIndex indexForInernalIdAndRow(int internalId, int row);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    struct DataCacheItem
    {
        ModelNode modelNode;
        PropertyName propertyName;
        int internalIndex = -1;
    };

    void setPropertyType(PropertyTypes type);
    void setFilter(const QString &filter);

    QList<ModelNode> nodeList() const;

    const std::vector<PropertyName> getProperties(const ModelNode &modelNode) const;
    ModelNode getModelNodeForId(const QString &id) const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QModelIndex ensureModelIndex(const ModelNode &node, int row) const;
    QModelIndex ensureModelIndex(const ModelNode &node, const PropertyName &name, int row) const;
    void testModel();
    const QList<ModelNode> allModelNodesWithIdsSortedByDisplayName() const;
    const std::vector<PropertyName> sortedAndFilteredPropertyNamesSignalsSlots(
        const ModelNode &modelNode) const;

    const std::vector<PropertyName> getDynamicProperties(const ModelNode &modelNode) const;
    const std::vector<PropertyName> sortedAndFilteredPropertyNames(const NodeMetaInfo &metaInfo,
                                                                   bool recursive = false) const;

    const std::vector<PropertyName> sortedAndFilteredSignalNames(const NodeMetaInfo &metaInfo,
                                                                 bool recursive = false) const;

    const std::vector<PropertyName> sortedAndFilteredSlotNames(const NodeMetaInfo &metaInfo,
                                                               bool recursive = false) const;

    const std::vector<PropertyName> sortedDotPropertyNames(const NodeMetaInfo &metaInfo,
                                                           const PropertyName &propertyName) const;

    const std::vector<PropertyName> sortedDotPropertySignals(const NodeMetaInfo &metaInfo,
                                                             const PropertyName &propertyName) const;

    const std::vector<PropertyName> sortedDotPropertySlots(const NodeMetaInfo &metaInfo,
                                                           const PropertyName &propertyName) const;

    const std::vector<PropertyName> sortedDotPropertyNamesSignalsSlots(
        const NodeMetaInfo &metaInfo, const PropertyName &propertyName) const;

    bool filterProperty(const PropertyName &name,
                        const PropertyMetaInfo &metaInfo,
                        bool recursive) const;

    ConnectionView *m_connectionView;

    mutable std::set<DataCacheItem> m_indexCache;
    mutable std::vector<DataCacheItem> m_indexHash;
    mutable int m_indexCount = 0;
    QList<ModelNode> m_nodeList;
    PropertyTypes m_type = AllTypes;
    QString m_filter;
};

class PropertyListProxyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PropertyListProxyModel(PropertyTreeModel *parent);
    void setRowandInternalId(int row, int internalId);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    ModelNode m_modelNode;
    PropertyName m_propertyName;
    QPersistentModelIndex m_parentIndex;

    PropertyTreeModel *m_treeModel = nullptr;
};

inline bool operator==(const PropertyTreeModel::DataCacheItem &lhs,
                       const PropertyTreeModel::DataCacheItem &rhs)
{
    return lhs.modelNode == rhs.modelNode && lhs.propertyName == rhs.propertyName;
}

inline bool operator<(const PropertyTreeModel::DataCacheItem &lhs,
                      const PropertyTreeModel::DataCacheItem &rhs)
{
    return (lhs.modelNode.id() + lhs.propertyName) < (rhs.modelNode.id() + rhs.propertyName);
}

class PropertyTreeModelDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(StudioQmlComboBoxBackend *name READ nameCombboBox CONSTANT)
    Q_PROPERTY(StudioQmlComboBoxBackend *id READ idCombboBox CONSTANT)

public:
    explicit PropertyTreeModelDelegate(ConnectionView *parent = nullptr);
    void setPropertyType(PropertyTreeModel::PropertyTypes type);
    void setup(const QString &id, const QString &name, bool *nameExists = nullptr);
    QString id() const;
    QString name() const;

signals:
    void commitData();

private:
    void handleNameChanged();
    void handleIdChanged();

    StudioQmlComboBoxBackend *nameCombboBox();
    StudioQmlComboBoxBackend *idCombboBox();

    StudioQmlComboBoxBackend m_nameCombboBox;
    StudioQmlComboBoxBackend m_idCombboBox;
    PropertyTreeModel::PropertyTypes m_type;
    PropertyTreeModel m_model;
};

} // namespace QmlDesigner