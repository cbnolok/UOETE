#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <QStandardItemModel>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>

#include "uoppackage/uoppackage.h"
#include "uoppackage/uopblock.h"
#include "uoppackage/uopfile.h"
#include "uoppackage/uophash.h"
#include "uoppackage/uoperror.h"
#include "uoclientfiles/uoart.h"
#include "cpputils/sysio.h"
#include "uostringdictionary.h"

#include <QDebug>

using ullong = unsigned long long;
const uint kInvalidSelectedIdx = uint(-1);

// Just write these things once
#define BUILD_UOPERROR_STR(msg)         std::string strError(msg); strError += "\n\nUOPPackage errors:\n"; strError += m_UOPErrorQueue->buildErrorsString()
#define SHOW_QMESSAGEBOX_ERROR(msg)     QMessageBox msgBox(QMessageBox::Critical, "Error", (msg), QMessageBox::Close); msgBox.exec()
#define SHOW_QMESSAGEBOX_ERROR_UOP(msg) BUILD_UOPERROR_STR(msg); SHOW_QMESSAGEBOX_ERROR(strError.c_str())

void setEnableWidgetsInLayout(QLayout* layout, bool val)
{
    for (int i = 0, max = layout->count(); i < max; ++i)
    {
        QLayoutItem *item = layout->itemAt(i);
        if (QLayout *sublayout = item->layout())
            setEnableWidgetsInLayout(sublayout, val);
        if (QWidget* widget = item->widget())
            widget->setEnabled(val);
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_selectedID = kInvalidSelectedIdx;
    m_UOPErrorQueue = std::make_unique<uopp::UOPError>();

    m_listModel = std::make_unique<QStandardItemModel>(0,0);
    ui->listView->setModel(m_listModel.get());
    connect(ui->listView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(onManual_listView_selectionChanged(const QModelIndex&,const QModelIndex&)));

    setEnableWidgetsInLayout(ui->gridLayout_various, false);
    setEnableWidgetsInLayout(ui->gridLayout_props_radarcol, false);
    setEnableWidgetsInLayout(ui->gridLayout_flags, false);
    ui->pushButton_save->setEnabled(false);

    std::ifstream cache("uoete_cache", std::ifstream::in);
    if (!cache.fail())
    {
        std::string strFolder;
        std::getline(cache, strFolder);
        ui->lineEdit_folder->setText(strFolder.c_str());
    }
    cache.close();
}

MainWindow::~MainWindow()
{
    std::ofstream cache("uoete_cache", std::ofstream::out);
    cache << ui->lineEdit_folder->text().toStdString();
    cache.close();

    delete ui;
}


// UI CONTROLS

void MainWindow::clearAll()
{
    uint selected = m_selectedID;
    m_selectedID = kInvalidSelectedIdx;

    ui->lineEdit_uopBlock->clear();
    ui->lineEdit_uopFile->clear();
    ui->lineEdit_version->clear();
    ui->lineEdit_stringDictOffset->clear();
    ui->lineEdit_ID->clear();
    ui->lineEdit_unkBool1->clear();
    ui->lineEdit_unkBool2->clear();
    ui->lineEdit_unkFloat3->clear();
    ui->lineEdit_unkFloat4->clear();
    ui->lineEdit_fixedZero->clear();
    ui->lineEdit_oldID->clear();
    ui->lineEdit_unkFloat5->clear();
    ui->lineEdit_type->clear();
    ui->lineEdit_unkByte6->clear();
    ui->lineEdit_unkDW7->clear();
    ui->lineEdit_unkDW8->clear();
    ui->lineEdit_lightRel1->clear();
    ui->lineEdit_lightRel2->clear();
    ui->lineEdit_unkDW9->clear();
    ui->lineEdit_facing->clear();
    ui->lineEdit_ECxStart->clear();
    ui->lineEdit_ECyStart->clear();
    ui->lineEdit_ECxEnd->clear();
    ui->lineEdit_ECyEnd->clear();
    ui->lineEdit_ECWidthMax->clear();
    ui->lineEdit_ECxOff->clear();
    ui->lineEdit_ECyOff->clear();
    ui->lineEdit_ECWidth->clear();
    ui->lineEdit_ECHeight->clear();
    ui->lineEdit_CCxStart->clear();
    ui->lineEdit_CCyStart->clear();
    ui->lineEdit_CCxEnd->clear();
    ui->lineEdit_CCyEnd->clear();
    ui->lineEdit_CCxOff->clear();
    ui->lineEdit_CCyOff->clear();
    ui->lineEdit_CCWidth->clear();
    ui->lineEdit_CCHeight->clear();

    ui->lineEdit_rc_a->setText("-");
    ui->lineEdit_rc_r->setText("-");
    ui->lineEdit_rc_g->setText("-");
    ui->lineEdit_rc_b->setText("-");
    ui->label_rc_view->setPixmap(QPixmap());

    ui->label_flags_val->setNum(0);
    ui->label_flags2_val->setNum(0);
    ui->checkBox_flag2_Background->setChecked(false);
    ui->checkBox_flag_Background->setChecked(false);
    ui->checkBox_flag2_Weapon->setChecked(false);
    ui->checkBox_flag_Weapon->setChecked(false);
    ui->checkBox_flag2_Transparent->setChecked(false);
    ui->checkBox_flag_Transparent->setChecked(false);
    ui->checkBox_flag2_Translucent->setChecked(false);
    ui->checkBox_flag_Translucent->setChecked(false);
    ui->checkBox_flag2_Wall->setChecked(false);
    ui->checkBox_flag_Wall->setChecked(false);
    ui->checkBox_flag2_Damaging->setChecked(false);
    ui->checkBox_flag_Damaging->setChecked(false);
    ui->checkBox_flag2_Impassable->setChecked(false);
    ui->checkBox_flag_Impassable->setChecked(false);
    ui->checkBox_flag2_Wet->setChecked(false);
    ui->checkBox_flag_Wet->setChecked(false);
    ui->checkBox_flag2_Ignored->setChecked(false);
    ui->checkBox_flag_Ignored->setChecked(false);
    ui->checkBox_flag2_Surface->setChecked(false);
    ui->checkBox_flag_Surface->setChecked(false);
    ui->checkBox_flag2_Bridge->setChecked(false);
    ui->checkBox_flag_Bridge->setChecked(false);
    ui->checkBox_flag2_Generic->setChecked(false);
    ui->checkBox_flag_Generic->setChecked(false);
    ui->checkBox_flag2_Window->setChecked(false);
    ui->checkBox_flag_Window->setChecked(false);
    ui->checkBox_flag2_NoShoot->setChecked(false);
    ui->checkBox_flag_NoShoot->setChecked(false);
    ui->checkBox_flag2_ArticleA->setChecked(false);
    ui->checkBox_flag_ArticleA->setChecked(false);
    ui->checkBox_flag2_ArticleAn->setChecked(false);
    ui->checkBox_flag_ArticleAn->setChecked(false);
    ui->checkBox_flag2_ArticleThe->setChecked(false);
    ui->checkBox_flag_ArticleThe->setChecked(false);
    ui->checkBox_flag2_Mongen->setChecked(false);
    ui->checkBox_flag_Mongen->setChecked(false);
    ui->checkBox_flag2_Foliage->setChecked(false);
    ui->checkBox_flag_Foliage->setChecked(false);
    ui->checkBox_flag2_PartialHue->setChecked(false);
    ui->checkBox_flag_PartialHue->setChecked(false);
    ui->checkBox_flag2_UseNewArt->setChecked(false);
    ui->checkBox_flag_UseNewArt->setChecked(false);
    ui->checkBox_flag2_Map->setChecked(false);
    ui->checkBox_flag_Map->setChecked(false);
    ui->checkBox_flag2_Container->setChecked(false);
    ui->checkBox_flag_Container->setChecked(false);
    ui->checkBox_flag2_Wearable->setChecked(false);
    ui->checkBox_flag_Wearable->setChecked(false);
    ui->checkBox_flag2_LightSource->setChecked(false);
    ui->checkBox_flag_LightSource->setChecked(false);
    ui->checkBox_flag2_Animation->setChecked(false);
    ui->checkBox_flag_Animation->setChecked(false);
    ui->checkBox_flag2_HoverOver->setChecked(false);
    ui->checkBox_flag_HoverOver->setChecked(false);
    ui->checkBox_flag2_ArtUsed->setChecked(false);
    ui->checkBox_flag_ArtUsed->setChecked(false);
    ui->checkBox_flag2_Armor->setChecked(false);
    ui->checkBox_flag_Armor->setChecked(false);
    ui->checkBox_flag2_Roof->setChecked(false);
    ui->checkBox_flag_Roof->setChecked(false);
    ui->checkBox_flag2_Door->setChecked(false);
    ui->checkBox_flag_Door->setChecked(false);
    ui->checkBox_flag2_StairBack->setChecked(false);
    ui->checkBox_flag_StairBack->setChecked(false);
    ui->checkBox_flag2_StairRight->setChecked(false);
    ui->checkBox_flag_StairRight->setChecked(false);
    ui->checkBox_flag2_NoHouse->setChecked(false);
    ui->checkBox_flag_NoHouse->setChecked(false);
    ui->checkBox_flag2_NoDraw->setChecked(false);
    ui->checkBox_flag_NoDraw->setChecked(false);
    ui->checkBox_flag2_Unused1->setChecked(false);
    ui->checkBox_flag_Unused1->setChecked(false);
    ui->checkBox_flag2_AlphaBlend->setChecked(false);
    ui->checkBox_flag_AlphaBlend->setChecked(false);
    ui->checkBox_flag2_NoShadow->setChecked(false);
    ui->checkBox_flag_NoShadow->setChecked(false);
    ui->checkBox_flag2_PixelBleed->setChecked(false);
    ui->checkBox_flag_PixelBleed->setChecked(false);
    ui->checkBox_flag2_Unused2->setChecked(false);
    ui->checkBox_flag_Unused2->setChecked(false);
    ui->checkBox_flag2_PlayAnimOnce->setChecked(false);
    ui->checkBox_flag_PlayAnimOnce->setChecked(false);
    ui->checkBox_flag2_MultiMovable->setChecked(false);
    ui->checkBox_flag_MultiMovable->setChecked(false);

    ui->lineEdit_prop2_AcVc->clear();
    ui->lineEdit_prop1_AcVc->clear();
    ui->lineEdit_prop2_Appearance->clear();
    ui->lineEdit_prop1_Appearance->clear();
    ui->lineEdit_prop2_Gender->clear();
    ui->lineEdit_prop1_Gender->clear();
    ui->lineEdit_prop2_Height->clear();
    ui->lineEdit_prop1_Height->clear();
    ui->lineEdit_prop2_Off_C8->clear();
    ui->lineEdit_prop1_Off_C8->clear();
    ui->lineEdit_prop2_Paperdoll->clear();
    ui->lineEdit_prop1_Paperdoll->clear();
    ui->lineEdit_prop2_Quality->clear();
    ui->lineEdit_prop1_Quality->clear();
    ui->lineEdit_prop2_Quantity->clear();
    ui->lineEdit_prop1_Quantity->clear();
    ui->lineEdit_prop2_Race->clear();
    ui->lineEdit_prop1_Race->clear();
    ui->lineEdit_prop2_Slot->clear();
    ui->lineEdit_prop1_Slot->clear();
    ui->lineEdit_prop2_Value->clear();
    ui->lineEdit_prop1_Value->clear();
    ui->lineEdit_prop2_Weight->clear();
    ui->lineEdit_prop1_Weight->clear();

    m_selectedID = selected;
}


// UI CONTROLS: transferring informations from the TileArt in memory to the UI

void MainWindow::populateFlags(const TileArtEntry &entry)
{
    ui->label_flags_val->setText(QString("0x%1").arg(entry.flags1, 0, 16));
    ui->label_flags2_val->setText(QString("0x%1").arg(entry.flags2, 0, 16));

    auto _populateFlagsCheckboxes = [this](ullong flags, bool isFull)
    {
        #define HASFLAG(flag) ((flags & static_cast<ullong>(flag)) == static_cast<ullong>(flag))
        #define CHECKPOPULATE_FLAG(flag, controlFull, control) \
            if (HASFLAG(flag)) { \
                if (isFull) ui->controlFull->setChecked(true); \
                else        ui->control->setChecked(true); \
            }
        CHECKPOPULATE_FLAG(TAEFlag::Background,  checkBox_flag2_Background,  checkBox_flag_Background);
        CHECKPOPULATE_FLAG(TAEFlag::Weapon,      checkBox_flag2_Weapon,      checkBox_flag_Weapon);
        CHECKPOPULATE_FLAG(TAEFlag::Transparent, checkBox_flag2_Transparent, checkBox_flag_Transparent);
        CHECKPOPULATE_FLAG(TAEFlag::Translucent, checkBox_flag2_Translucent, checkBox_flag_Translucent);
        CHECKPOPULATE_FLAG(TAEFlag::Wall,        checkBox_flag2_Wall,        checkBox_flag_Wall);
        CHECKPOPULATE_FLAG(TAEFlag::Damaging,    checkBox_flag2_Damaging,    checkBox_flag_Damaging);
        CHECKPOPULATE_FLAG(TAEFlag::Impassable,  checkBox_flag2_Impassable,  checkBox_flag_Impassable);
        CHECKPOPULATE_FLAG(TAEFlag::Wet,         checkBox_flag2_Wet,         checkBox_flag_Wet);
        CHECKPOPULATE_FLAG(TAEFlag::Ignored,     checkBox_flag2_Ignored,     checkBox_flag_Ignored);
        CHECKPOPULATE_FLAG(TAEFlag::Surface,     checkBox_flag2_Surface,     checkBox_flag_Surface);
        CHECKPOPULATE_FLAG(TAEFlag::Bridge,      checkBox_flag2_Bridge,      checkBox_flag_Bridge);
        CHECKPOPULATE_FLAG(TAEFlag::Generic,     checkBox_flag2_Generic,     checkBox_flag_Generic);
        CHECKPOPULATE_FLAG(TAEFlag::Window,      checkBox_flag2_Window,      checkBox_flag_Window);
        CHECKPOPULATE_FLAG(TAEFlag::NoShoot,     checkBox_flag2_NoShoot,     checkBox_flag_NoShoot);
        CHECKPOPULATE_FLAG(TAEFlag::ArticleThe,  checkBox_flag2_ArticleThe,  checkBox_flag_ArticleThe)
        else CHECKPOPULATE_FLAG(TAEFlag::ArticleA,   checkBox_flag2_ArticleA,    checkBox_flag_ArticleA)
        else CHECKPOPULATE_FLAG(TAEFlag::ArticleAn,  checkBox_flag2_ArticleAn,   checkBox_flag_ArticleAn);
        CHECKPOPULATE_FLAG(TAEFlag::Mongen,      checkBox_flag2_Mongen,      checkBox_flag_Mongen);
        CHECKPOPULATE_FLAG(TAEFlag::Foliage,     checkBox_flag2_Foliage,     checkBox_flag_Foliage);
        CHECKPOPULATE_FLAG(TAEFlag::PartialHue,  checkBox_flag2_PartialHue,  checkBox_flag_PartialHue);
        CHECKPOPULATE_FLAG(TAEFlag::UseNewArt,   checkBox_flag2_UseNewArt,   checkBox_flag_UseNewArt);
        CHECKPOPULATE_FLAG(TAEFlag::Map,         checkBox_flag2_Map,         checkBox_flag_Map);
        CHECKPOPULATE_FLAG(TAEFlag::Container,   checkBox_flag2_Container,   checkBox_flag_Container);
        CHECKPOPULATE_FLAG(TAEFlag::Wearable,    checkBox_flag2_Wearable,    checkBox_flag_Wearable);
        CHECKPOPULATE_FLAG(TAEFlag::LightSource, checkBox_flag2_LightSource, checkBox_flag_LightSource);
        CHECKPOPULATE_FLAG(TAEFlag::Animation,   checkBox_flag2_Animation,   checkBox_flag_Animation);
        CHECKPOPULATE_FLAG(TAEFlag::HoverOver,   checkBox_flag2_HoverOver,   checkBox_flag_HoverOver);
        CHECKPOPULATE_FLAG(TAEFlag::ArtUsed,     checkBox_flag2_ArtUsed,     checkBox_flag_ArtUsed);
        CHECKPOPULATE_FLAG(TAEFlag::Armor,       checkBox_flag2_Armor,       checkBox_flag_Armor);
        CHECKPOPULATE_FLAG(TAEFlag::Roof,        checkBox_flag2_Roof,        checkBox_flag_Roof);
        CHECKPOPULATE_FLAG(TAEFlag::Door,        checkBox_flag2_Door,        checkBox_flag_Door);
        CHECKPOPULATE_FLAG(TAEFlag::StairBack,   checkBox_flag2_StairBack,   checkBox_flag_StairBack);
        CHECKPOPULATE_FLAG(TAEFlag::StairRight,  checkBox_flag2_StairRight,  checkBox_flag_StairRight);
        CHECKPOPULATE_FLAG(TAEFlag::NoHouse,     checkBox_flag2_NoHouse,     checkBox_flag_NoHouse);
        CHECKPOPULATE_FLAG(TAEFlag::NoDraw,      checkBox_flag2_NoDraw,      checkBox_flag_NoDraw);
        CHECKPOPULATE_FLAG(TAEFlag::Unused1,     checkBox_flag2_Unused1,     checkBox_flag_Unused1);
        CHECKPOPULATE_FLAG(TAEFlag::AlphaBlend,  checkBox_flag2_AlphaBlend,  checkBox_flag_AlphaBlend);
        CHECKPOPULATE_FLAG(TAEFlag::NoShadow,    checkBox_flag2_NoShadow,    checkBox_flag_NoShadow);
        CHECKPOPULATE_FLAG(TAEFlag::PixelBleed,  checkBox_flag2_PixelBleed,  checkBox_flag_PixelBleed);
        CHECKPOPULATE_FLAG(TAEFlag::Unused2,     checkBox_flag2_Unused2,     checkBox_flag_Unused2);
        CHECKPOPULATE_FLAG(TAEFlag::PlayAnimOnce,checkBox_flag2_PlayAnimOnce,checkBox_flag_PlayAnimOnce);
        CHECKPOPULATE_FLAG(TAEFlag::MultiMovable,checkBox_flag2_MultiMovable,checkBox_flag_MultiMovable);

        #undef HASFLAG
        #undef CHECKPOPULATE_FLAG
    };

    _populateFlagsCheckboxes(entry.flags1, false);
    _populateFlagsCheckboxes(entry.flags2, true);
}

void MainWindow::populatePropVal(const TAEPropID &prop, const QString &strVal, bool isProp2)
{
#define CHECKPOPULATE_PROPVAL(propID, control2, control) \
    if (prop == propID) { \
        if (isProp2)    ui->control2->setText(strVal); \
        else            ui->control->setText(strVal); \
        return; \
    }

    CHECKPOPULATE_PROPVAL(TAEPropID::AcVc,          lineEdit_prop2_AcVc,        lineEdit_prop1_AcVc);
    CHECKPOPULATE_PROPVAL(TAEPropID::Appearance,    lineEdit_prop2_Appearance,  lineEdit_prop1_Appearance);
    CHECKPOPULATE_PROPVAL(TAEPropID::Gender,        lineEdit_prop2_Gender,      lineEdit_prop1_Gender);
    CHECKPOPULATE_PROPVAL(TAEPropID::Height,        lineEdit_prop2_Height,      lineEdit_prop1_Height);
    CHECKPOPULATE_PROPVAL(TAEPropID::Off_C8,        lineEdit_prop2_Off_C8,      lineEdit_prop1_Off_C8);
    CHECKPOPULATE_PROPVAL(TAEPropID::Paperdoll,     lineEdit_prop2_Paperdoll,   lineEdit_prop1_Paperdoll);
    CHECKPOPULATE_PROPVAL(TAEPropID::Quantity,      lineEdit_prop2_Quantity,    lineEdit_prop1_Quantity);
    CHECKPOPULATE_PROPVAL(TAEPropID::Race,          lineEdit_prop2_Race,        lineEdit_prop1_Race);
    CHECKPOPULATE_PROPVAL(TAEPropID::Slot,          lineEdit_prop2_Slot,        lineEdit_prop1_Slot);
    CHECKPOPULATE_PROPVAL(TAEPropID::Value,         lineEdit_prop2_Value,       lineEdit_prop1_Value);
    CHECKPOPULATE_PROPVAL(TAEPropID::Weight,        lineEdit_prop2_Weight,      lineEdit_prop1_Weight);

#undef CHECKPOPULATE_PROPVAL
}

void MainWindow::populateEntry(const TileArtEntry& entry)
{
    clearAll();

    ui->lineEdit_uopBlock->setText(QString::number(entry.uopBlock));
    ui->lineEdit_uopFile->setText(QString::number(entry.uopFile));
    ui->lineEdit_version->setText(QString::number(entry.version));
    ui->lineEdit_stringDictOffset->setText(QString::number(entry.stringDictOff));
    ui->lineEdit_ID->setText(QString::number(entry.tileID) + " (0x" + QString::number(entry.tileID, 16) + ")");
    ui->lineEdit_unkBool1->setText(QString::number(entry.unkBool1));
    ui->lineEdit_unkBool2->setText(QString::number(entry.unkBool2));
    ui->lineEdit_unkFloat3->setText(QString::number(entry.unkFloat3));
    ui->lineEdit_unkFloat4->setText(QString::number(entry.unkFloat4));
    ui->lineEdit_fixedZero->setText(QString::number(entry.fixedZero));
    ui->lineEdit_oldID->setText(QString::number(entry.oldID));
    ui->lineEdit_unkFloat5->setText(QString::number(entry.unkFloat5));
    ui->lineEdit_type->setText(QString::number(entry.type));
    ui->lineEdit_unkByte6->setText(QString::number(entry.unkByte6));
    ui->lineEdit_unkDW7->setText(QString::number(entry.unkDW7));
    ui->lineEdit_unkDW8->setText(QString::number(entry.unkDW8));
    ui->lineEdit_lightRel1->setText(QString::number(entry.light1));
    ui->lineEdit_lightRel2->setText(QString::number(entry.light2));
    ui->lineEdit_unkDW9->setText(QString::number(entry.unkDW9));

    populateFlags(entry);

    ui->lineEdit_facing->setText(QString::number(entry.facing));

    ui->lineEdit_ECxStart->setText(QString::number(entry.ECImgOffset.xStart));
    ui->lineEdit_ECyStart->setText(QString::number(entry.ECImgOffset.yStart));
    ui->lineEdit_ECxEnd->setText(QString::number(entry.ECImgOffset.xEnd));
    ui->lineEdit_ECyEnd->setText(QString::number(entry.ECImgOffset.yEnd));
    ui->lineEdit_ECxOff->setText(QString::number(entry.ECImgOffset.xOff));
    ui->lineEdit_ECyOff->setText(QString::number(entry.ECImgOffset.yOff));
    ui->lineEdit_ECWidth->setText(QString::number(entry.ECImgOffset.xEnd - entry.ECImgOffset.xStart));
    ui->lineEdit_ECHeight->setText(QString::number(entry.ECImgOffset.yEnd - entry.ECImgOffset.yStart));
    ui->lineEdit_ECWidthMax->setText(QString::number(entry.ECImgOffset.xEnd - entry.ECImgOffset.xStart + entry.ECImgOffset.xOff));
    ui->lineEdit_ECHeightMax->setText(QString::number(entry.ECImgOffset.yEnd - entry.ECImgOffset.yStart + entry.ECImgOffset.yOff));

    ui->lineEdit_CCxStart->setText(QString::number(entry.CCImgOffset.xStart));
    ui->lineEdit_CCyStart->setText(QString::number(entry.CCImgOffset.yStart));
    ui->lineEdit_CCxEnd->setText(QString::number(entry.CCImgOffset.xEnd));
    ui->lineEdit_CCyEnd->setText(QString::number(entry.CCImgOffset.yEnd));
    ui->lineEdit_CCxOff->setText(QString::number(entry.CCImgOffset.xOff));
    ui->lineEdit_CCyOff->setText(QString::number(entry.CCImgOffset.yOff));
    ui->lineEdit_CCWidth->setText(QString::number(entry.CCImgOffset.xEnd - entry.CCImgOffset.xStart));
    ui->lineEdit_CCHeight->setText(QString::number(entry.CCImgOffset.yEnd - entry.CCImgOffset.yStart));

    for (unsigned i = 0; i < entry.propCount1; ++i)
    {
        populatePropVal(entry.propVector1[i].ID, QString::number(entry.propVector1[i].val), false);
    }
    for (unsigned i = 0; i < entry.propCount2; ++i)
    {
        populatePropVal(entry.propVector2[i].ID, QString::number(entry.propVector2[i].val), true);
    }
}


// UI CONTROLS: transferring informations from the UI to the TileArt in memory

#define SETSTRVAL_COMMON    if (m_selectedID == kInvalidSelectedIdx) return; \
                            /*qDebug() << "selected strval" << m_selectedID;*/ \
                            TileArtEntry& entry = m_TAEntries.at(m_selectedID)
#define SETSTRVAL_INT(field)    SETSTRVAL_COMMON;   entry.field = arg1.toInt()
#define SETSTRVAL_UINT(field)   SETSTRVAL_COMMON;   entry.field = arg1.toUInt()
#define SETSTRVAL_UCHAR(field)  SETSTRVAL_COMMON;   ushort tmp = arg1.toUShort(); entry.field = static_cast<uchar>((tmp >= UINT8_MAX) ? UINT8_MAX : tmp);
#define SETSTRVAL_FLOAT(field)  SETSTRVAL_COMMON;   entry.field = arg1.toFloat()
#define SETSTRVAL_BOOL(field)   SETSTRVAL_COMMON;   entry.field = bool(arg1.toInt())

void MainWindow::on_lineEdit_version_textChanged(const QString &arg1)           { SETSTRVAL_UINT(version); }
void MainWindow::on_lineEdit_stringDictOffset_textChanged(const QString &arg1)  { SETSTRVAL_UINT(stringDictOff); }
//void MainWindow::on_lineEdit_CCWidth_textChanged(const QString &arg1)           {  }
//void MainWindow::on_lineEdit_uopBlock_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_lightRel1_textChanged(const QString &arg1)         { SETSTRVAL_FLOAT(light1); }
void MainWindow::on_lineEdit_unkFloat4_textChanged(const QString &arg1)         { SETSTRVAL_FLOAT(unkFloat4); }
void MainWindow::on_lineEdit_ECxOff_textChanged(const QString &arg1)            { SETSTRVAL_INT(ECImgOffset.xOff); }
void MainWindow::on_lineEdit_unkFloat3_textChanged(const QString &arg1)         { SETSTRVAL_FLOAT(unkFloat3); }
void MainWindow::on_lineEdit_ECyOff_textChanged(const QString &arg1)            { SETSTRVAL_INT(ECImgOffset.yOff); }
void MainWindow::on_lineEdit_CCxOff_textChanged(const QString &arg1)            { SETSTRVAL_INT(CCImgOffset.xOff); }
void MainWindow::on_lineEdit_ID_textChanged(const QString &arg1)                { SETSTRVAL_UINT(tileID); }
//void MainWindow::on_lineEdit_ECHeight_textChanged(const QString &arg1);
//void MainWindow::on_lineEdit_ECHeightMax_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_unkDW9_textChanged(const QString &arg1)            { SETSTRVAL_UINT(unkDW9); }
void MainWindow::on_lineEdit_ECyEnd_textChanged(const QString &arg1)            { SETSTRVAL_INT(ECImgOffset.yEnd); }
//void MainWindow::on_lineEdit_uopFile_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_CCxEnd_textChanged(const QString &arg1)            { SETSTRVAL_INT(CCImgOffset.xEnd); }
void MainWindow::on_lineEdit_unkFloat5_textChanged(const QString &arg1)         { SETSTRVAL_FLOAT(unkFloat5); }
void MainWindow::on_lineEdit_oldID_textChanged(const QString &arg1)             { SETSTRVAL_INT(oldID); }   // or uint?
//void MainWindow::on_lineEdit_ECWidth_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_type_textChanged(const QString &arg1)              { SETSTRVAL_INT(type); }
void MainWindow::on_lineEdit_unkByte6_textChanged(const QString &arg1)          { SETSTRVAL_UCHAR(unkByte6); }
void MainWindow::on_lineEdit_CCyOff_textChanged(const QString &arg1)            { SETSTRVAL_INT(CCImgOffset.yOff); }
void MainWindow::on_lineEdit_unkBool2_textChanged(const QString &arg1)          { SETSTRVAL_BOOL(unkBool2); }
void MainWindow::on_lineEdit_CCxStart_textChanged(const QString &arg1)          { SETSTRVAL_INT(CCImgOffset.xStart); }
void MainWindow::on_lineEdit_facing_textChanged(const QString &arg1)            { SETSTRVAL_INT(facing); }
//void MainWindow::on_lineEdit_CCHeight_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_CCyEnd_textChanged(const QString &arg1)            { SETSTRVAL_INT(CCImgOffset.yEnd); }
void MainWindow::on_lineEdit_fixedZero_textChanged(const QString &arg1)         { SETSTRVAL_INT(fixedZero); }
void MainWindow::on_lineEdit_CCyStart_textChanged(const QString &arg1)          { SETSTRVAL_INT(ECImgOffset.yStart); }
void MainWindow::on_lineEdit_unkBool1_textChanged(const QString &arg1)          { SETSTRVAL_BOOL(unkBool1); }
void MainWindow::on_lineEdit_unkDW8_textChanged(const QString &arg1)            { SETSTRVAL_INT(unkDW8); }  // or int?
void MainWindow::on_lineEdit_lightRel2_textChanged(const QString &arg1)         { SETSTRVAL_FLOAT(light2); }
void MainWindow::on_lineEdit_unkDW7_textChanged(const QString &arg1)            { SETSTRVAL_INT(unkDW8); }  // or int?
//void MainWindow::on_lineEdit_ECWidthMax_textChanged(const QString &arg1);
void MainWindow::on_lineEdit_ECxStart_textChanged(const QString &arg1)          { SETSTRVAL_INT(ECImgOffset.xStart); }
void MainWindow::on_lineEdit_ECxEnd_textChanged(const QString &arg1)            { SETSTRVAL_INT(ECImgOffset.xEnd); }
void MainWindow::on_lineEdit_ECyStart_textChanged(const QString &arg1)          { SETSTRVAL_INT(ECImgOffset.yStart); }

#undef SETSTRVAL_COMMON
#undef SETSTRVAL_INT
#undef SETSTRVAL_UINT
#undef SETSTRVAL_FLOAT
#undef SETSTRVAL_BOOL
#undef SETSTRVAL_UCHAR

#define SETPROP(isFull, propID) if (m_selectedID == kInvalidSelectedIdx) return; \
                                TileArtEntry& entry = m_TAEntries.at(m_selectedID); \
                                _SetProp(entry, arg1, isFull, propID)
void _SetProp (TileArtEntry& entry, const QString &arg1, bool isFull, TAEPropID propID)
{
    const bool emptyProp = arg1.isNull();
    unsigned char& count = isFull ? entry.propCount2 : entry.propCount1;
    std::vector<TAEProp>& propVec = isFull ? entry.propVector2 : entry.propVector1;
    for (auto it = propVec.begin(), end = propVec.end(); it != end; ++it)
    {
        TAEProp& curProp = *it;
        if (curProp.ID != propID)
            continue;

        if (emptyProp)
        {
            // Empty val: remove the prop
            propVec.erase(it);
            if (count <= 0)
                qDebug() << "count <= 0 for ID, isFull, propID =" << entry.tileID << isFull << int(propID);
            --count;
        }
        else
        {
            // Update the prop val
            curProp.val = arg1.toUInt();
        }
        return;
    }
    // Not found: add it
    propVec.emplace_back(TAEProp{propID, arg1.toUInt()});
    ++count;
}

void MainWindow::on_lineEdit_prop1_Height_textChanged(const QString &arg1)      { SETPROP(false, TAEPropID::Height); }
void MainWindow::on_lineEdit_prop1_Value_textChanged(const QString &arg1)       { SETPROP(false, TAEPropID::Value); }
void MainWindow::on_lineEdit_prop1_Off_C8_textChanged(const QString &arg1)      { SETPROP(false, TAEPropID::Off_C8); }
void MainWindow::on_lineEdit_prop1_AcVc_textChanged(const QString &arg1)        { SETPROP(false, TAEPropID::AcVc); }
void MainWindow::on_lineEdit_prop1_Quality_textChanged(const QString &arg1)     { SETPROP(false, TAEPropID::Quality); }
void MainWindow::on_lineEdit_prop1_Race_textChanged(const QString &arg1)        { SETPROP(false, TAEPropID::Race); }
void MainWindow::on_lineEdit_prop1_Quantity_textChanged(const QString &arg1)    { SETPROP(false, TAEPropID::Quantity); }
void MainWindow::on_lineEdit_prop1_Paperdoll_textChanged(const QString &arg1)   { SETPROP(false, TAEPropID::Paperdoll); }
void MainWindow::on_lineEdit_prop1_Gender_textChanged(const QString &arg1)      { SETPROP(false, TAEPropID::Gender); }
void MainWindow::on_lineEdit_prop1_Weight_textChanged(const QString &arg1)      { SETPROP(false, TAEPropID::Weight); }
void MainWindow::on_lineEdit_prop1_Slot_textChanged(const QString &arg1)        { SETPROP(false, TAEPropID::Slot); }
void MainWindow::on_lineEdit_prop1_Appearance_textChanged(const QString &arg1)  { SETPROP(false, TAEPropID::Appearance); }

void MainWindow::on_lineEdit_prop2_Value_textChanged(const QString &arg1)       { SETPROP(true, TAEPropID::Value); }
void MainWindow::on_lineEdit_prop2_Weight_textChanged(const QString &arg1)      { SETPROP(true, TAEPropID::Weight); }
void MainWindow::on_lineEdit_prop2_Quantity_textChanged(const QString &arg1)    { SETPROP(true, TAEPropID::Quantity); }
void MainWindow::on_lineEdit_prop2_Quality_textChanged(const QString &arg1)     { SETPROP(true, TAEPropID::Quality); }
void MainWindow::on_lineEdit_prop2_Height_textChanged(const QString &arg1)      { SETPROP(true, TAEPropID::Height); }
void MainWindow::on_lineEdit_prop2_Slot_textChanged(const QString &arg1)        { SETPROP(true, TAEPropID::Slot); }
void MainWindow::on_lineEdit_prop2_Off_C8_textChanged(const QString &arg1)      { SETPROP(true, TAEPropID::Off_C8); }
void MainWindow::on_lineEdit_prop2_Appearance_textChanged(const QString &arg1)  { SETPROP(true, TAEPropID::Appearance); }
void MainWindow::on_lineEdit_prop2_Race_textChanged(const QString &arg1)        { SETPROP(true, TAEPropID::Race); }
void MainWindow::on_lineEdit_prop2_Gender_textChanged(const QString &arg1)      { SETPROP(true, TAEPropID::Gender); }
void MainWindow::on_lineEdit_prop2_Paperdoll_textChanged(const QString &arg1)   { SETPROP(true, TAEPropID::Paperdoll); }
void MainWindow::on_lineEdit_prop2_AcVc_textChanged(const QString &arg1)        { SETPROP(true, TAEPropID::AcVc); }

#undef SETPROP

#define SETFLAG(flagVar, flagType)  if (m_selectedID == kInvalidSelectedIdx) return; \
                                    /*qDebug() << "selected check" << m_selectedID;*/ \
                                    auto flagTypeTmp = static_cast<ullong>(flagType); \
                                    TileArtEntry& entry = m_TAEntries.at(m_selectedID); \
                                    if (arg1 == static_cast<int>(Qt::Unchecked)) entry.flagVar &= ~flagTypeTmp; \
                                    else   entry.flagVar |= flagTypeTmp; \
                                    populateFlags(entry);

void MainWindow::on_checkBox_flag_Background_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::Background)  }
void MainWindow::on_checkBox_flag_Weapon_stateChanged(int arg1)         { SETFLAG(flags1, TAEFlag::Weapon)  }
void MainWindow::on_checkBox_flag_Transparent_stateChanged(int arg1)    { SETFLAG(flags1, TAEFlag::Transparent)  }
void MainWindow::on_checkBox_flag_Translucent_stateChanged(int arg1)    { SETFLAG(flags1, TAEFlag::Translucent)  }
void MainWindow::on_checkBox_flag_Wall_stateChanged(int arg1)           { SETFLAG(flags1, TAEFlag::Wall)  }
void MainWindow::on_checkBox_flag_Damaging_stateChanged(int arg1)       { SETFLAG(flags1, TAEFlag::Damaging)  }
void MainWindow::on_checkBox_flag_Impassable_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::Impassable)  }
void MainWindow::on_checkBox_flag_Wet_stateChanged(int arg1)            { SETFLAG(flags1, TAEFlag::Wet)  }
void MainWindow::on_checkBox_flag_Ignored_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Ignored)  }
void MainWindow::on_checkBox_flag_Surface_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Surface)  }
void MainWindow::on_checkBox_flag_Bridge_stateChanged(int arg1)         { SETFLAG(flags1, TAEFlag::Bridge)  }
void MainWindow::on_checkBox_flag_Generic_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Generic)  }
void MainWindow::on_checkBox_flag_Window_stateChanged(int arg1)         { SETFLAG(flags1, TAEFlag::Window)  }
void MainWindow::on_checkBox_flag_NoShoot_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::NoShoot)  }
void MainWindow::on_checkBox_flag_ArticleA_stateChanged(int arg1)       { SETFLAG(flags1, TAEFlag::ArticleA)  }
void MainWindow::on_checkBox_flag_ArticleAn_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::ArticleAn)  }
void MainWindow::on_checkBox_flag_ArticleThe_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::ArticleThe)  }
void MainWindow::on_checkBox_flag_Mongen_stateChanged(int arg1)         { SETFLAG(flags1, TAEFlag::Mongen)  }
void MainWindow::on_checkBox_flag_Foliage_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Foliage)  }
void MainWindow::on_checkBox_flag_PartialHue_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::PartialHue)  }
void MainWindow::on_checkBox_flag_UseNewArt_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::UseNewArt)  }
void MainWindow::on_checkBox_flag_Map_stateChanged(int arg1)            { SETFLAG(flags1, TAEFlag::Map)  }
void MainWindow::on_checkBox_flag_Container_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::Container)  }
void MainWindow::on_checkBox_flag_Wearable_stateChanged(int arg1)       { SETFLAG(flags1, TAEFlag::Wearable)  }
void MainWindow::on_checkBox_flag_LightSource_stateChanged(int arg1)    { SETFLAG(flags1, TAEFlag::LightSource)  }
void MainWindow::on_checkBox_flag_Animation_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::Animation)  }
void MainWindow::on_checkBox_flag_HoverOver_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::HoverOver)  }
void MainWindow::on_checkBox_flag_ArtUsed_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::ArtUsed)  }
void MainWindow::on_checkBox_flag_Armor_stateChanged(int arg1)          { SETFLAG(flags1, TAEFlag::Armor)  }
void MainWindow::on_checkBox_flag_Roof_stateChanged(int arg1)           { SETFLAG(flags1, TAEFlag::Roof)  }
void MainWindow::on_checkBox_flag_Door_stateChanged(int arg1)           { SETFLAG(flags1, TAEFlag::Door)  }
void MainWindow::on_checkBox_flag_StairBack_stateChanged(int arg1)      { SETFLAG(flags1, TAEFlag::StairBack)  }
void MainWindow::on_checkBox_flag_StairRight_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::StairRight)  }
void MainWindow::on_checkBox_flag_NoHouse_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::NoHouse)  }
void MainWindow::on_checkBox_flag_NoDraw_stateChanged(int arg1)         { SETFLAG(flags1, TAEFlag::NoDraw)  }
void MainWindow::on_checkBox_flag_Unused1_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Unused1)  }
void MainWindow::on_checkBox_flag_AlphaBlend_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::AlphaBlend)  }
void MainWindow::on_checkBox_flag_Unused2_stateChanged(int arg1)        { SETFLAG(flags1, TAEFlag::Unused2)  }
void MainWindow::on_checkBox_flag_NoShadow_stateChanged(int arg1)       { SETFLAG(flags1, TAEFlag::NoShadow)  }
void MainWindow::on_checkBox_flag_PixelBleed_stateChanged(int arg1)     { SETFLAG(flags1, TAEFlag::PixelBleed)  }
void MainWindow::on_checkBox_flag_PlayAnimOnce_stateChanged(int arg1)   { SETFLAG(flags1, TAEFlag::PlayAnimOnce)  }
void MainWindow::on_checkBox_flag_MultiMovable_stateChanged(int arg1)   { SETFLAG(flags1, TAEFlag::MultiMovable)  }

void MainWindow::on_checkBox_flag2_Background_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::Background)  }
void MainWindow::on_checkBox_flag2_Weapon_stateChanged(int arg1)         { SETFLAG(flags2, TAEFlag::Weapon)  }
void MainWindow::on_checkBox_flag2_Transparent_stateChanged(int arg1)    { SETFLAG(flags2, TAEFlag::Transparent)  }
void MainWindow::on_checkBox_flag2_Translucent_stateChanged(int arg1)    { SETFLAG(flags2, TAEFlag::Translucent)  }
void MainWindow::on_checkBox_flag2_Wall_stateChanged(int arg1)           { SETFLAG(flags2, TAEFlag::Wall)  }
void MainWindow::on_checkBox_flag2_Damaging_stateChanged(int arg1)       { SETFLAG(flags2, TAEFlag::Damaging)  }
void MainWindow::on_checkBox_flag2_Impassable_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::Impassable)  }
void MainWindow::on_checkBox_flag2_Wet_stateChanged(int arg1)            { SETFLAG(flags2, TAEFlag::Wet)  }
void MainWindow::on_checkBox_flag2_Ignored_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Ignored)  }
void MainWindow::on_checkBox_flag2_Surface_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Surface)  }
void MainWindow::on_checkBox_flag2_Bridge_stateChanged(int arg1)         { SETFLAG(flags2, TAEFlag::Bridge)  }
void MainWindow::on_checkBox_flag2_Generic_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Generic)  }
void MainWindow::on_checkBox_flag2_Window_stateChanged(int arg1)         { SETFLAG(flags2, TAEFlag::Window)  }
void MainWindow::on_checkBox_flag2_NoShoot_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::NoShoot)  }
void MainWindow::on_checkBox_flag2_ArticleA_stateChanged(int arg1)       { SETFLAG(flags2, TAEFlag::ArticleA)  }
void MainWindow::on_checkBox_flag2_ArticleAn_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::ArticleAn)  }
void MainWindow::on_checkBox_flag2_ArticleThe_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::ArticleThe)  }
void MainWindow::on_checkBox_flag2_Mongen_stateChanged(int arg1)         { SETFLAG(flags2, TAEFlag::Mongen)  }
void MainWindow::on_checkBox_flag2_Foliage_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Foliage)  }
void MainWindow::on_checkBox_flag2_PartialHue_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::PartialHue)  }
void MainWindow::on_checkBox_flag2_UseNewArt_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::UseNewArt)  }
void MainWindow::on_checkBox_flag2_Map_stateChanged(int arg1)            { SETFLAG(flags2, TAEFlag::Map)  }
void MainWindow::on_checkBox_flag2_Container_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::Container)  }
void MainWindow::on_checkBox_flag2_Wearable_stateChanged(int arg1)       { SETFLAG(flags2, TAEFlag::Wearable)  }
void MainWindow::on_checkBox_flag2_LightSource_stateChanged(int arg1)    { SETFLAG(flags2, TAEFlag::LightSource)  }
void MainWindow::on_checkBox_flag2_Animation_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::Animation)  }
void MainWindow::on_checkBox_flag2_HoverOver_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::HoverOver)  }
void MainWindow::on_checkBox_flag2_ArtUsed_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::ArtUsed)  }
void MainWindow::on_checkBox_flag2_Armor_stateChanged(int arg1)          { SETFLAG(flags2, TAEFlag::Armor)  }
void MainWindow::on_checkBox_flag2_Roof_stateChanged(int arg1)           { SETFLAG(flags2, TAEFlag::Roof)  }
void MainWindow::on_checkBox_flag2_Door_stateChanged(int arg1)           { SETFLAG(flags2, TAEFlag::Door)  }
void MainWindow::on_checkBox_flag2_StairBack_stateChanged(int arg1)      { SETFLAG(flags2, TAEFlag::StairBack)  }
void MainWindow::on_checkBox_flag2_StairRight_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::StairRight)  }
void MainWindow::on_checkBox_flag2_NoHouse_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::NoHouse)  }
void MainWindow::on_checkBox_flag2_NoDraw_stateChanged(int arg1)         { SETFLAG(flags2, TAEFlag::NoDraw)  }
void MainWindow::on_checkBox_flag2_Unused1_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Unused1)  }
void MainWindow::on_checkBox_flag2_AlphaBlend_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::AlphaBlend)  }
void MainWindow::on_checkBox_flag2_Unused2_stateChanged(int arg1)        { SETFLAG(flags2, TAEFlag::Unused2)  }
void MainWindow::on_checkBox_flag2_NoShadow_stateChanged(int arg1)       { SETFLAG(flags2, TAEFlag::NoShadow)  }
void MainWindow::on_checkBox_flag2_PixelBleed_stateChanged(int arg1)     { SETFLAG(flags2, TAEFlag::PixelBleed)  }
void MainWindow::on_checkBox_flag2_PlayAnimOnce_stateChanged(int arg1)   { SETFLAG(flags2, TAEFlag::PlayAnimOnce)  }
void MainWindow::on_checkBox_flag2_MultiMovable_stateChanged(int arg1)   { SETFLAG(flags2, TAEFlag::MultiMovable)  }

#undef SETFLAG


// Update RadarCol preview when the color value has changed

void MainWindow::updateRadarColPreview()
{
    const TileArtEntry& entry = m_TAEntries.at(m_selectedID);
    QPixmap radarcolPix(1, 1);
    radarcolPix.fill(qRgba(entry.radarcol.r, entry.radarcol.g, entry.radarcol.b, entry.radarcol.a));
    ui->label_rc_view->setPixmap(radarcolPix);
}

#define SETRADARCOL(byte) \
    if (m_selectedID == kInvalidSelectedIdx) return; \
    ushort val = arg1.toUShort(); \
    if (val > 255) return; \
    m_TAEntries.at(m_selectedID).radarcol.byte = uchar(val); \
    updateRadarColPreview()
void MainWindow::on_lineEdit_rc_a_textChanged(const QString &arg1) {
    SETRADARCOL(a);
}
void MainWindow::on_lineEdit_rc_r_textChanged(const QString &arg1) {
    SETRADARCOL(r);
}
void MainWindow::on_lineEdit_rc_g_textChanged(const QString &arg1) {
    SETRADARCOL(g);
}
void MainWindow::on_lineEdit_rc_b_textChanged(const QString &arg1) {
    SETRADARCOL(b);
}
#undef SETRADARCOL


// Load TileArt.uop into memory

void MainWindow::loadPackage()
{
    setEnableWidgetsInLayout(ui->gridLayout_various, false);
    setEnableWidgetsInLayout(ui->gridLayout_props_radarcol, false);
    setEnableWidgetsInLayout(ui->gridLayout_flags, false);
    ui->pushButton_save->setEnabled(false);

    if (ui->lineEdit_folder->text().isEmpty())
    {
        SHOW_QMESSAGEBOX_ERROR("Empty client path");
        return;
    }
    m_ECPath = ui->lineEdit_folder->text().toStdString();
    standardizePath(m_ECPath);
    if (!isValidDirectory(m_ECPath))
    {
        SHOW_QMESSAGEBOX_ERROR("Invalid path");
        return;
    }

    m_listModel->clear();

    m_stringDictionary = std::make_unique<UOStringDictionary>();
    if (!m_stringDictionary->load(m_ECPath + "string_dictionary.uop"))
    {
        SHOW_QMESSAGEBOX_ERROR("Can't find string_dictionary.uop");
        m_stringDictionary.reset(nullptr);
        return;
    }

    m_TAPackage = std::make_unique<uopp::UOPPackage>();
    m_UOPErrorQueue->clear();
    if (!m_TAPackage->load(m_ECPath + "Tileart.uop", m_UOPErrorQueue.get()))
    {
        SHOW_QMESSAGEBOX_ERROR_UOP("Can't find Tileart.uop");
        m_TAPackage.reset(nullptr);
        return;
    }
    std::vector<uopp::UOPBlock*> blocks = m_TAPackage->getBlocks();
    if (blocks.empty())
    {
        SHOW_QMESSAGEBOX_ERROR("Empty Tileart.uop?");
        m_TAPackage.reset(nullptr);
        return;
    }
    m_TAEntries.clear();

    // For the item/texture preview
    m_UOArt = std::make_unique<uocf::UOArt>(m_ECPath);

    // Data for managing the progress bar
    size_t estimatedFilesNumber = blocks.size() * blocks[0]->getFileCount();
    estimatedFilesNumber *= 2;  // Since we have two for loops.
    ui->progressBar->setValue(0);
    int iProgressPercent = 0;
    size_t iFilesTot = 0;

    // Load the entries
    std::ifstream fin = m_TAPackage->getOpenedStream();
    for (int iBlock = 0; iBlock < int(blocks.size()); ++iBlock)
    {
        uopp::UOPBlock* curBlock = blocks[iBlock];
        for (int iFile = 0; iFile < curBlock->getFileCount(); ++iFile)
        {
            uopp::UOPFile* curFile = curBlock->getFile(iFile);
            curFile->readData(fin, m_UOPErrorQueue.get());
            TileArtEntry entry = TileArtEntry::load(curFile, m_UOPErrorQueue.get(), iBlock, iFile);

            if (entry.tileID >= 200000000)
                qDebug() << "High ID?" << entry.tileID;

            //if (!m_TAEntries.count(entry.tileID))
                //m_TAEntries[entry.tileID] = entry;
            m_TAEntries.insert(std::make_pair(entry.tileID, std::move(entry)));
            //else
            //    qDebug() << "duplicated" << entry.tileID << iBlock << iFile;

            ++iFilesTot;
            int iProgressPercentCur = int((iFilesTot * 100) / estimatedFilesNumber);
            if (iProgressPercentCur != iProgressPercent)
            {
                iProgressPercent = iProgressPercentCur;
                if (iProgressPercent <= 50)
                    ui->progressBar->setValue(iProgressPercent);
            }
        }
    }
    fin.close();

    // Populate the list
    ui->listView->setUpdatesEnabled(false);
    for (auto const& pair : m_TAEntries)
    {
        const TileArtEntry& entry = pair.second;

        QStandardItem* qsItem = new QStandardItem(
                    QString("0x") + QString::number(entry.tileID, 16) + " - " + QString::number(entry.tileID) + " - " +
                    m_stringDictionary->getString(entry.stringDictOff - 1).c_str() );
        m_listItemsMap.insert(std::make_pair(qsItem, entry.tileID));
        m_listModel->appendRow(qsItem);

        ++iFilesTot;
        int iProgressPercentCur = int((iFilesTot * 100) / estimatedFilesNumber);
        if (iProgressPercentCur != iProgressPercent)
        {
            iProgressPercent = iProgressPercentCur;
            if (iProgressPercent <= 100)
                ui->progressBar->setValue(iProgressPercent);
        }
       }
    ui->listView->setUpdatesEnabled(true);
    ui->progressBar->setValue(0);

    setEnableWidgetsInLayout(ui->gridLayout_various, true);
    setEnableWidgetsInLayout(ui->gridLayout_props_radarcol, true);
    setEnableWidgetsInLayout(ui->gridLayout_flags, true);
    ui->pushButton_save->setEnabled(true);
}


// Populate the entries and show the pictures

void MainWindow::onManual_listView_selectionChanged(const QModelIndex &selected, const QModelIndex& /* UNUSED deselected */)
{
    if (m_listModel->rowCount() == 0)     // Empty list, can't proceed.
        return;

    QStandardItem *qsItem = m_listModel->itemFromIndex(selected);
    if (!m_listItemsMap.count(qsItem))
        return;

    m_selectedID = m_listItemsMap.at(qsItem);
    //qDebug() << "selected" << m_selectedID;
    const TileArtEntry& entry = m_TAEntries.at(m_selectedID);
    populateEntry(entry);

    // Get the textures id
    std::vector<std::vector<TextureItem>> textureIDs = TileArtEntry::getTextureItemVector(entry.textureVector, m_stringDictionary.get());

    // Draw the textures
    if (ui->graphicsView->scene() != nullptr)
        delete ui->graphicsView->scene();
    auto* scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    scene->clear();

    int yOff = 0;
    for (const std::vector<TextureItem>& textureIDsSub : textureIDs)
    {
        int xOff = 0;
        int yOffBiggest = 0;
        for (const TextureItem& texItem : textureIDsSub)
        {
            if (texItem.type == TextureType::Undefined)
                continue;

            bool drawLegacy = (texItem.type == TextureType::TileArtLegacy);
            QImage* artImg = m_UOArt->drawArtEnhanced(drawLegacy, uocf::UOArt::kItemsOffset + texItem.id, 0, false);
            if (artImg != nullptr)
            {
                QPixmap artPixmap = QPixmap::fromImage(*artImg);
                if (texItem.id < 1000000)
                {
                    // Draw the offset box only for the art textures, not for the noise ones
                    QPainter qPainter(&artPixmap);
                    qPainter.setBrush(Qt::NoBrush);
                    qPainter.setPen(Qt::red);
                    const TAEImgOffset& bounds = drawLegacy ? entry.CCImgOffset : entry.ECImgOffset;
                    const int width = bounds.xEnd - bounds.xStart, height = bounds.yEnd - bounds.yStart;
                    qPainter.drawRect(bounds.xStart, bounds.yStart, width, height);
                    qPainter.end();
                }
                else
                {
                    // The noise texture is too big, let's scale the pixmap
                    artPixmap = artPixmap.scaledToHeight(artPixmap.height() / 2);
                }

                auto* artPixmapItem = new QGraphicsPixmapItem(artPixmap);
                artPixmapItem->setPos(xOff, yOff);
                scene->addItem(artPixmapItem);
                xOff += artPixmap.width() + 20;
                if (artPixmap.height() > yOffBiggest)
                    yOffBiggest = artPixmap.height();
            }
            //delete artImg;
        }
        yOff += yOffBiggest + 20;
    }

    // Draw RadarCol
    ui->lineEdit_rc_a->setText(QString::number(entry.radarcol.a));
    ui->lineEdit_rc_r->setText(QString::number(entry.radarcol.r));
    ui->lineEdit_rc_g->setText(QString::number(entry.radarcol.g));
    ui->lineEdit_rc_b->setText(QString::number(entry.radarcol.b));
    updateRadarColPreview();
}

void MainWindow::on_pushButton_load_clicked()
{
    loadPackage();
}

// Browse the folder

void MainWindow::on_pushButton_browse_clicked()
{
    QFileDialog dlg;
    dlg.setOption(QFileDialog::ShowDirsOnly);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setViewMode(QFileDialog::List);
    if (isValidDirectory(ui->lineEdit_folder->text().toStdString()))
         dlg.setDirectory(ui->lineEdit_folder->text());

    if (dlg.exec())
        ui->lineEdit_folder->setText(dlg.directory().absolutePath());
}

// -- SAVE

void MainWindow::on_pushButton_save_clicked()
{
    if (ui->lineEdit_rc_a->text().toUInt() > 255) {
        SHOW_QMESSAGEBOX_ERROR("Invalid RadarCol A value! It needs to be <= 255 and > 0.");
        return;
    }
    if (ui->lineEdit_rc_r->text().toUInt() > 255) {
        SHOW_QMESSAGEBOX_ERROR("Invalid RadarCol R value! It needs to be <= 255 and > 0.");
        return;
    }
    if (ui->lineEdit_rc_g->text().toUInt() > 255) {
        SHOW_QMESSAGEBOX_ERROR("Invalid RadarCol G value! It needs to be <= 255 and > 0.");
        return;
    }
    if (ui->lineEdit_rc_b->text().toUInt() > 255) {
        SHOW_QMESSAGEBOX_ERROR("Invalid RadarCol B value! It needs to be <= 255 and > 0.");
        return;
    }

    if (ui->checkBox_backup->checkState() != Qt::Unchecked)
    {
        std::string oldName = (m_ECPath + "Tileart.uop");
        std::string newName = (m_ECPath + "Tileart_UOETEbku_" + QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss").toStdString() + ".uop");
        if (rename(oldName.c_str(), newName.c_str()) != 0)
        {
            SHOW_QMESSAGEBOX_ERROR("Can't make a backup for Tileart.uop! Aborting the save process.");
            return;
        }
    }

    ui->progressBar->setValue(0);
    int iProgressPercent = 0;
    size_t iEntryMax = m_TAEntries.size();
    size_t iEntry = 0;

    for (auto const& pair : m_TAEntries)
    {
        uopp::UOPFile* file = m_TAPackage->getFileByIndex(pair.second.uopBlock, pair.second.uopFile);

        std::vector<char> decompressedFileData;
        if (!file->unpack(&decompressedFileData, m_UOPErrorQueue.get()))
        {
            SHOW_QMESSAGEBOX_ERROR_UOP(
                        QString("Can't unpack from Tileart.uop file %1 from block %2.").arg(pair.second.uopBlock).arg(pair.second.uopFile).toStdString());
            return;
        }

        std::vector<char> newDecompressedFileData;
        newDecompressedFileData.resize(decompressedFileData.size()*2);

        const TileArtEntry& entry = pair.second;
        size_t dataOffset = 0;
        const char *decompressedFileDataPtr = decompressedFileData.data();
        char *newDecompressedFileDataPtr = newDecompressedFileData.data();

#define WRITEMEM(src, size) \
    memcpy(static_cast<void*>(newDecompressedFileDataPtr+dataOffset), static_cast<const void*>(&entry.src), (size)); \
    dataOffset += (size);

#define WRITEMEMFULL(src, size) \
    memcpy(static_cast<void*>(newDecompressedFileDataPtr+dataOffset), static_cast<const void*>(src), (size)); \
    dataOffset += (size);

        WRITEMEM(version, 2);
        WRITEMEM(stringDictOff, 4);
        WRITEMEM(tileID, 4);
        WRITEMEM(unkBool1, 1);
        WRITEMEM(unkBool2, 1);
        WRITEMEM(unkFloat3, 4);
        WRITEMEM(unkFloat4, 4);
        WRITEMEM(fixedZero, 4);
        WRITEMEM(oldID, 4);
        WRITEMEM(unkFloat5, 4);
        WRITEMEM(type, 4);
        WRITEMEM(unkByte6, 1);
        WRITEMEM(unkDW7, 4);
        WRITEMEM(unkDW8, 4);
        WRITEMEM(light1, 4);
        WRITEMEM(light2, 4);
        WRITEMEM(unkDW9, 4);

        WRITEMEM(flags1, 8);
        WRITEMEM(flags2, 8);

        WRITEMEM(facing, 4);

        WRITEMEM(ECImgOffset.xStart, 4);
        WRITEMEM(ECImgOffset.yStart, 4);
        WRITEMEM(ECImgOffset.xEnd, 4);
        WRITEMEM(ECImgOffset.yEnd, 4);
        WRITEMEM(ECImgOffset.xOff, 4);
        WRITEMEM(ECImgOffset.yOff, 4);

        WRITEMEM(CCImgOffset.xStart, 4);
        WRITEMEM(CCImgOffset.yStart, 4);
        WRITEMEM(CCImgOffset.xEnd, 4);
        WRITEMEM(CCImgOffset.yEnd, 4);
        WRITEMEM(CCImgOffset.xOff, 4);
        WRITEMEM(CCImgOffset.yOff, 4);

        WRITEMEM(propCount1, 1);
        for (unsigned i = 0; i < entry.propCount1; ++i)
        {
            WRITEMEM(propVector1[i].ID, 1);
            WRITEMEM(propVector1[i].val, 4);
        }
        WRITEMEM(propCount2, 1);
        for (unsigned i = 0; i < entry.propCount2; ++i)
        {
            WRITEMEM(propVector2[i].ID, 1);
            WRITEMEM(propVector2[i].val, 4);
        }

        // copy the rest of the file
        WRITEMEMFULL(decompressedFileDataPtr + entry.stackAliasCount_offset, file->getDecompressedSize() - entry.stackAliasCount_offset);
        newDecompressedFileData.resize(dataOffset); // shrink the buffer to fit, since we allocated more memory than needed

        // overwrite the radarcol
        dataOffset = entry.radarcol_offset;
        WRITEMEM(radarcol.r, 1);
        WRITEMEM(radarcol.g, 1);
        WRITEMEM(radarcol.b, 1);
        WRITEMEM(radarcol.a, 1);

        // transfer the new file content to the uop file in memory
        if (!file->compressAndReplaceData(&newDecompressedFileData, uopp::CompressionFlag::ZLib, m_UOPErrorQueue.get()))
        {
            SHOW_QMESSAGEBOX_ERROR_UOP("Can't replace data");
            return;
        }

        ++iEntry;
        int iProgressPercentCur = int((iEntry * 100) / iEntryMax);
        if (iProgressPercentCur != iProgressPercent)
        {
            iProgressPercent = iProgressPercentCur;
            ui->progressBar->setValue(iProgressPercent);
        }
#undef WRITEMEM
#undef WRITEMEM_FULL
    }

    if (!m_TAPackage->finalizeAndSave(m_ECPath + "Tileart.uop", m_UOPErrorQueue.get()))
    {
        SHOW_QMESSAGEBOX_ERROR_UOP("Can't save to Tileart.uop");
        return;
    }
    ui->progressBar->setValue(0);
    QApplication::beep();
}


void MainWindow::on_pushButton_setAll_newArt_clicked()
{
    if (m_TAEntries.empty() || (m_selectedID == kInvalidSelectedIdx))
        return;

    ui->pushButton_setAll_newArt->setEnabled(false);
    for (auto& pair : m_TAEntries)
    {
        TileArtEntry& entry = pair.second;
        if (entry.tileID >= 200000000)
            qDebug() << "High ID?" << entry.tileID;

        // Make (nearly) everything use the KR graphics.
        //qDebug() << entry.tileID << "pre" << QString::number(entry.flags2, 16);
        if (entry.tileID < 16071)
            entry.flags2 = (entry.flags2 & (~ullong(TAEFlag::ArtUsed))) | ullong(TAEFlag::UseNewArt);
        //qDebug() << entry.tileID << "post" << QString::number(entry.flags2, 16);
        //if (entry.tileID > 30)
        //    break;
    }
    const TileArtEntry& entry = m_TAEntries.at(m_selectedID);
    populateEntry(entry);
    ui->pushButton_setAll_newArt->setEnabled(true);
}

