/****************************************************************************
 *
 *   (c) 2017 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "LifetimeChecker.h"
#include "AirspaceRulesetsProvider.h"
#include "AirMapSharedState.h"

#include <QGeoCoordinate>
#include <QStringList>

#include <airmap/rulesets.h>

/**
 * @file AirMapRulesetsManager.h
 * Class to download rulesets from AirMap
 */

//-----------------------------------------------------------------------------
class AirMapRuleFeature : public AirspaceRuleFeature
{
    Q_OBJECT
public:

    AirMapRuleFeature(QObject* parent = NULL);
    AirMapRuleFeature(airmap::RuleSet::Feature feature, QObject* parent = NULL);

    quint32         id              () override { return (quint32)_feature.id; }
    Type            type            () override;
    Unit            unit            () override;
    Measurement     measurement     () override;
    QString         name            () override { return QString::fromStdString(_feature.name);  }
    QString         description     () override { return QString::fromStdString(_feature.description);  }
    QVariant        value           () override { return _value; }
    void            setValue        (const QVariant val) override { _value = val; emit valueChanged(); }
private:
    airmap::RuleSet::Feature _feature;
    QVariant _value;
};

//-----------------------------------------------------------------------------
class AirMapRule : public AirspaceRule
{
    Q_OBJECT
public:

    AirMapRule(QObject* parent = NULL);
    AirMapRule(const airmap::RuleSet::Rule& rule, QObject* parent = NULL);

    int             order           () { return (int)_rule.display_order; }
    Status          status          () override;
    QString         shortText       () override { return QString::fromStdString(_rule.short_text);  }
    QString         description     () override { return QString::fromStdString(_rule.description); }

private:
    airmap::RuleSet::Rule _rule;
};

//-----------------------------------------------------------------------------
class AirMapRuleSet : public AirspaceRuleSet
{
    Q_OBJECT
    friend class AirMapRulesetsManager;
public:
    AirMapRuleSet                   (QObject* parent = NULL);
    ~AirMapRuleSet                  ();
    QString         id              () override { return _id; }
    QString         description     () override { return _description; }
    bool            isDefault       () override { return _isDefault; }
    QString         name            () override { return _name; }
    QString         shortName       () override { return _shortName; }
    SelectionType   selectionType   () override { return _selectionType; }
    bool            selected        () override { return _selected; }
    void            setSelected     (bool sel) override { _selected = sel; emit selectedChanged(); }
    QmlObjectListModel* rules       () override { return &_rules; }
private:
    QString             _id;
    QString             _description;
    bool                _isDefault;
    bool                _selected;
    QString             _name;
    QString             _shortName;
    SelectionType       _selectionType;
    QmlObjectListModel  _rules;
};

//-----------------------------------------------------------------------------
class AirMapRulesetsManager : public AirspaceRulesetsProvider, public LifetimeChecker
{
    Q_OBJECT
public:
    AirMapRulesetsManager       (AirMapSharedState& shared);

    bool                valid       () override { return _valid; }
    QmlObjectListModel* ruleSets    () override { return &_ruleSets; }
    QmlObjectListModel* features    () override { return &_features; }
    QString         selectedRuleSets() override;

    void                setROI      (const QGeoCoordinate& center) override;

    //-- Selected rules
    QStringList         rulesetsIDs ();

signals:
    void        error               (const QString& what, const QString& airmapdMessage, const QString& airmapdDetails);

private slots:
    void        _selectedChanged    ();

private:
    enum class State {
        Idle,
        RetrieveItems,
    };
    bool                            _valid;
    State                           _state = State::Idle;
    AirMapSharedState&              _shared;
    QmlObjectListModel              _ruleSets;
    //-- TODO: Connect to AirMapRuleSet::selectedChanged and rebuild features based on it.
    QmlObjectListModel              _features;
};

