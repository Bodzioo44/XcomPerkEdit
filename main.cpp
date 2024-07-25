#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}



int mainu()
{

//testing
//checkpoints[0].checkpoint_table[260].properties[0].properties[2].properties[4].int_values[0]
xcom::saved_game save = xcom::read_xcom_save("../Testing/save475");
xcom::checkpoint_chunk_table& checkpoint_table = save.checkpoints;
xcom::checkpoint_chunk& checkpoint = checkpoint_table[0];
xcom::checkpoint_table& table = checkpoint.checkpoints;
xcom::checkpoint& soldieru = table[260];
xcom::property_list& properties = soldieru.properties;

auto m_kChar = static_cast<xcom::struct_property*> (properties[0].get());
cout << m_kChar->kind_string() << endl;
cout << m_kChar->name << endl;

auto aUpgrades = static_cast<xcom::static_array_property*> (m_kChar->properties[3].get());
cout << aUpgrades->kind_string() << endl;
cout << aUpgrades->name << endl;

//auto aUpgrades_property_list = aUpgrades->properties;
cout << aUpgrades->properties[0]->kind_string() << endl;
cout << aUpgrades->properties.size() << endl;
auto aUpgrades_int0 = static_cast<xcom::int_property*> (aUpgrades->properties[0].get());
aUpgrades_int0->value = 1;
cout << aUpgrades_int0->value << endl;



// auto properites3 = static_cast<xcom::struct_property*> (properties2->properties[2].get());
// cout << properites3->kind_string() << endl;
// auto properties4 = static_cast<xcom::struct_property*> (properites3->properties[4].get());
// cout << properties4->kind_string() << endl;
// auto properties5 = static_cast<xcom::int_property*> (properties4->properties[0].get());
// cout << properties5->value << endl;
// //struct_property* properties2 = static_cast<struct_property*> (&properties[0]);

}