#include "Utils.h"

std::string Extract::class_type(const xcom::property_list* properties) {
    xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
    xcom::struct_property& kClass = static_cast<xcom::struct_property&> (*m_kSoldier.properties[11]);
    return static_cast<xcom::string_property&> (*kClass.properties[0]).str.str;
}

std::string Extract::status(const xcom::property_list* properties) {
    xcom::enum_property& m_eStatus = static_cast<xcom::enum_property&> (*properties->at(3));
    return m_eStatus.value.name;
}

int Extract::rank(const xcom::property_list* properties) {
    xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
    return static_cast<xcom::int_property&> (*m_kSoldier.properties[4]).value;
}

QString Extract::full_name(const xcom::property_list* properties) {
    xcom::struct_property& m_kSoldier = static_cast<xcom::struct_property&> (*properties->at(1));
    xcom::string_property& strFirstName = static_cast<xcom::string_property&> (*m_kSoldier.properties[1]);
    xcom::string_property& strLastName = static_cast<xcom::string_property&> (*m_kSoldier.properties[2]);
    xcom::string_property& strNickName = static_cast<xcom::string_property&> (*m_kSoldier.properties[3]);
    return QString::fromStdString(strFirstName.str.str + " \"" + strNickName.str.str + "\" " + strLastName.str.str);
}

PerkSet Extract::perks(const xcom::property_list* properties) {
    xcom::struct_property& m_kChar = static_cast<xcom::struct_property&> (*properties->at(0));
    xcom::static_array_property& aUpgrades = static_cast<xcom::static_array_property&> (*m_kChar.properties[3]);

    // std::string class_path = ":/res/perks/" + class_type(properties) + ".txt";
    QFile class_file(QString::fromStdString(":/res/perks/" + class_type(properties) + ".txt"));
    QFile psi_file(":/res/perks/PSI.txt");
    QFile gene_file(":/res/perks/GeneMods.txt");
    QFile officer_file(":/res/perks/Officer.txt");

    for (const xcom::property_ptr& index_value : aUpgrades.properties) {
        xcom::int_property& int_prop = static_cast<xcom::int_property&> (*index_value);
    }
}
