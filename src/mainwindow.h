#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <fstream>
#include <map>

#include <QMainWindow>

#include "uotileart.h"


class QStandardItemModel;
class QStandardItem;

namespace uopp {
class UOPPackage;
class UOPError;
}

namespace uocf {
class UOArt;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


    void clearAll();
    void populateFlags(const TileArtEntry& entry);
    void populatePropVal(const TAEPropID& prop, const QString& strVal, bool isProp2);
    void populateEntry(const TileArtEntry& entry);
    void updateRadarColPreview();

    const TileArtEntry& loadEntry(std::ifstream& fin, uint block, uint file);
    void loadPackage();

    std::string m_ECPath;

    std::unique_ptr<UOStringDictionary> m_stringDictionary;
    std::unique_ptr<uopp::UOPError> m_UOPErrorQueue;
    std::unique_ptr<uopp::UOPPackage> m_TAPackage;
    std::map<uint, TileArtEntry> m_TAEntries;

    uint m_selectedID;

    std::unique_ptr<QStandardItemModel> m_listModel;
    std::map<QStandardItem*, uint> m_listItemsMap;

    std::unique_ptr<uocf::UOArt> m_UOArt;

private slots:
    void onManual_listView_selectionChanged(const QModelIndex &selected, const QModelIndex& /* UNUSED deselected */);

    void on_lineEdit_version_textChanged(const QString &arg1);
    void on_lineEdit_stringDictOffset_textChanged(const QString &arg1);
    //void on_lineEdit_CCWidth_textChanged(const QString &arg1);
    //void on_lineEdit_uopBlock_textChanged(const QString &arg1);
    void on_lineEdit_lightRel1_textChanged(const QString &arg1);
    void on_lineEdit_unkFloat4_textChanged(const QString &arg1);
    void on_lineEdit_ECxOff_textChanged(const QString &arg1);
    void on_lineEdit_unkFloat3_textChanged(const QString &arg1);
    void on_lineEdit_ECyOff_textChanged(const QString &arg1);
    void on_lineEdit_CCxOff_textChanged(const QString &arg1);
    void on_lineEdit_ID_textChanged(const QString &arg1);
    //void on_lineEdit_ECHeight_textChanged(const QString &arg1);
    //void on_lineEdit_ECHeightMax_textChanged(const QString &arg1);
    void on_lineEdit_unkDW9_textChanged(const QString &arg1);
    void on_lineEdit_ECyEnd_textChanged(const QString &arg1);
    //void on_lineEdit_uopFile_textChanged(const QString &arg1);
    void on_lineEdit_CCxEnd_textChanged(const QString &arg1);
    void on_lineEdit_unkFloat5_textChanged(const QString &arg1);
    void on_lineEdit_oldID_textChanged(const QString &arg1);
    //void on_lineEdit_ECWidth_textChanged(const QString &arg1);
    void on_lineEdit_type_textChanged(const QString &arg1);
    void on_lineEdit_unkByte6_textChanged(const QString &arg1);
    void on_lineEdit_CCyOff_textChanged(const QString &arg1);
    void on_lineEdit_unkBool2_textChanged(const QString &arg1);
    void on_lineEdit_CCxStart_textChanged(const QString &arg1);
    void on_lineEdit_facing_textChanged(const QString &arg1);
    //void on_lineEdit_CCHeight_textChanged(const QString &arg1);
    void on_lineEdit_CCyEnd_textChanged(const QString &arg1);
    void on_lineEdit_fixedZero_textChanged(const QString &arg1);
    void on_lineEdit_CCyStart_textChanged(const QString &arg1);
    void on_lineEdit_unkBool1_textChanged(const QString &arg1);
    void on_lineEdit_unkDW8_textChanged(const QString &arg1);
    void on_lineEdit_lightRel2_textChanged(const QString &arg1);
    void on_lineEdit_unkDW7_textChanged(const QString &arg1);
    //void on_lineEdit_ECWidthMax_textChanged(const QString &arg1);
    void on_lineEdit_ECxStart_textChanged(const QString &arg1);
    void on_lineEdit_ECxEnd_textChanged(const QString &arg1);
    void on_lineEdit_ECyStart_textChanged(const QString &arg1);

    void on_lineEdit_prop1_Height_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Value_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Off_C8_textChanged(const QString &arg1);
    void on_lineEdit_prop1_AcVc_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Quality_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Race_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Quantity_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Paperdoll_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Gender_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Weight_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Slot_textChanged(const QString &arg1);
    void on_lineEdit_prop1_Appearance_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Value_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Weight_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Quantity_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Quality_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Height_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Slot_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Off_C8_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Appearance_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Race_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Gender_textChanged(const QString &arg1);
    void on_lineEdit_prop2_Paperdoll_textChanged(const QString &arg1);
    void on_lineEdit_prop2_AcVc_textChanged(const QString &arg1);

    void on_lineEdit_rc_a_textChanged(const QString &arg1);
    void on_lineEdit_rc_r_textChanged(const QString &arg1);
    void on_lineEdit_rc_g_textChanged(const QString &arg1);
    void on_lineEdit_rc_b_textChanged(const QString &arg1);

    void on_checkBox_flag_Background_stateChanged(int arg1);
    void on_checkBox_flag_Weapon_stateChanged(int arg1);
    void on_checkBox_flag_Transparent_stateChanged(int arg1);
    void on_checkBox_flag_Translucent_stateChanged(int arg1);
    void on_checkBox_flag_Wall_stateChanged(int arg1);
    void on_checkBox_flag_Damaging_stateChanged(int arg1);
    void on_checkBox_flag_Impassable_stateChanged(int arg1);
    void on_checkBox_flag_Wet_stateChanged(int arg1);
    void on_checkBox_flag_Ignored_stateChanged(int arg1);
    void on_checkBox_flag_Surface_stateChanged(int arg1);
    void on_checkBox_flag_Bridge_stateChanged(int arg1);
    void on_checkBox_flag_Generic_stateChanged(int arg1);
    void on_checkBox_flag_Window_stateChanged(int arg1);
    void on_checkBox_flag_NoShoot_stateChanged(int arg1);
    void on_checkBox_flag_ArticleA_stateChanged(int arg1);
    void on_checkBox_flag_ArticleAn_stateChanged(int arg1);
    void on_checkBox_flag_ArticleThe_stateChanged(int arg1);
    void on_checkBox_flag_Mongen_stateChanged(int arg1);
    void on_checkBox_flag_Foliage_stateChanged(int arg1);
    void on_checkBox_flag_PartialHue_stateChanged(int arg1);
    void on_checkBox_flag_UseNewArt_stateChanged(int arg1);
    void on_checkBox_flag_Map_stateChanged(int arg1);
    void on_checkBox_flag_Container_stateChanged(int arg1);
    void on_checkBox_flag_Wearable_stateChanged(int arg1);
    void on_checkBox_flag_LightSource_stateChanged(int arg1);
    void on_checkBox_flag_Animation_stateChanged(int arg1);
    void on_checkBox_flag_HoverOver_stateChanged(int arg1);
    void on_checkBox_flag_ArtUsed_stateChanged(int arg1);
    void on_checkBox_flag_Armor_stateChanged(int arg1);
    void on_checkBox_flag_Roof_stateChanged(int arg1);
    void on_checkBox_flag_Door_stateChanged(int arg1);
    void on_checkBox_flag_StairBack_stateChanged(int arg1);
    void on_checkBox_flag_StairRight_stateChanged(int arg1);
    void on_checkBox_flag_NoHouse_stateChanged(int arg1);
    void on_checkBox_flag_NoDraw_stateChanged(int arg1);
    void on_checkBox_flag_Unused1_stateChanged(int arg1);
    void on_checkBox_flag_AlphaBlend_stateChanged(int arg1);
    void on_checkBox_flag_Unused2_stateChanged(int arg1);
    void on_checkBox_flag_NoShadow_stateChanged(int arg1);
    void on_checkBox_flag_PixelBleed_stateChanged(int arg1);
    void on_checkBox_flag_PlayAnimOnce_stateChanged(int arg1);
    void on_checkBox_flag_MultiMovable_stateChanged(int arg1);

    void on_checkBox_flag2_Background_stateChanged(int arg1);
    void on_checkBox_flag2_Weapon_stateChanged(int arg1);
    void on_checkBox_flag2_Transparent_stateChanged(int arg1);
    void on_checkBox_flag2_Translucent_stateChanged(int arg1);
    void on_checkBox_flag2_Wall_stateChanged(int arg1);
    void on_checkBox_flag2_Damaging_stateChanged(int arg1);
    void on_checkBox_flag2_Impassable_stateChanged(int arg1);
    void on_checkBox_flag2_Wet_stateChanged(int arg1);
    void on_checkBox_flag2_Ignored_stateChanged(int arg1);
    void on_checkBox_flag2_Surface_stateChanged(int arg1);
    void on_checkBox_flag2_Bridge_stateChanged(int arg1);
    void on_checkBox_flag2_Generic_stateChanged(int arg1);
    void on_checkBox_flag2_Window_stateChanged(int arg1);
    void on_checkBox_flag2_NoShoot_stateChanged(int arg1);
    void on_checkBox_flag2_ArticleA_stateChanged(int arg1);
    void on_checkBox_flag2_ArticleAn_stateChanged(int arg1);
    void on_checkBox_flag2_ArticleThe_stateChanged(int arg1);
    void on_checkBox_flag2_Mongen_stateChanged(int arg1);
    void on_checkBox_flag2_Foliage_stateChanged(int arg1);
    void on_checkBox_flag2_PartialHue_stateChanged(int arg1);
    void on_checkBox_flag2_UseNewArt_stateChanged(int arg1);
    void on_checkBox_flag2_Map_stateChanged(int arg1);
    void on_checkBox_flag2_Container_stateChanged(int arg1);
    void on_checkBox_flag2_Wearable_stateChanged(int arg1);
    void on_checkBox_flag2_LightSource_stateChanged(int arg1);
    void on_checkBox_flag2_Animation_stateChanged(int arg1);
    void on_checkBox_flag2_HoverOver_stateChanged(int arg1);
    void on_checkBox_flag2_ArtUsed_stateChanged(int arg1);
    void on_checkBox_flag2_Armor_stateChanged(int arg1);
    void on_checkBox_flag2_Roof_stateChanged(int arg1);
    void on_checkBox_flag2_Door_stateChanged(int arg1);
    void on_checkBox_flag2_StairBack_stateChanged(int arg1);
    void on_checkBox_flag2_StairRight_stateChanged(int arg1);
    void on_checkBox_flag2_NoHouse_stateChanged(int arg1);
    void on_checkBox_flag2_NoDraw_stateChanged(int arg1);
    void on_checkBox_flag2_Unused1_stateChanged(int arg1);
    void on_checkBox_flag2_AlphaBlend_stateChanged(int arg1);
    void on_checkBox_flag2_Unused2_stateChanged(int arg1);
    void on_checkBox_flag2_NoShadow_stateChanged(int arg1);
    void on_checkBox_flag2_PixelBleed_stateChanged(int arg1);
    void on_checkBox_flag2_PlayAnimOnce_stateChanged(int arg1);
    void on_checkBox_flag2_MultiMovable_stateChanged(int arg1);

    //void on_lineEdit_search_textChanged(const QString &arg1);
    //void on_lineEdit_folder_textChanged(const QString &arg1);

    void on_pushButton_load_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_browse_clicked();

    void on_pushButton_setAll_newArt_clicked();
};

#endif // MAINWINDOW_H
