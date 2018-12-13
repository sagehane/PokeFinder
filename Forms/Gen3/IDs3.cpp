/*
 * This file is part of PokéFinder
 * Copyright (C) 2017 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "IDs3.hpp"
#include "ui_IDs3.h"

IDs3::IDs3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IDs3)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(Qt::Widget | Qt::MSWindowsFixedSizeDialogHint);

    setupModels();
}

IDs3::~IDs3()
{
    delete ui;
    delete xdcolo;
    delete frlge;
    delete rs;
}

void IDs3::setupModels()
{
    ui->textBoxPIDFRLGE->setValues(InputType::Seed32Bit);
    ui->textBoxTIDFRLGE->setValues(InputType::TIDSID);
    ui->textBoxSIDFRLGE->setValues(InputType::TIDSID);
    ui->textBoxMinFrameFRLGE->setValues(InputType::Frame32Bit);
    ui->textBoxMaxFrameFRLGE->setValues(InputType::Frame32Bit);

    ui->textBoxPIDRS->setValues(InputType::Seed32Bit);
    ui->textBoxTIDRS->setValues(InputType::TIDSID);
    ui->textBoxSIDRS->setValues(InputType::TIDSID);
    ui->textBoxInitSeedRS->setValues(InputType::Seed16Bit);
    ui->textBoxMinFrameRS->setValues(InputType::Frame32Bit);
    ui->textBoxMaxFrameRS->setValues(InputType::Frame32Bit);

    ui->textBoxPIDXD->setValues(InputType::Seed32Bit);
    ui->textBoxTIDXD->setValues(InputType::TIDSID);
    ui->textBoxSIDXD->setValues(InputType::TIDSID);
    ui->textBoxSeedXD->setValues(InputType::Seed32Bit);
    ui->textBoxMinFrameXD->setValues(InputType::Frame32Bit);
    ui->textBoxMaxFrameXD->setValues(InputType::Frame32Bit);

    ui->dateTimeEdit->setDisplayFormat(QLocale::system().dateTimeFormat(QLocale::ShortFormat));

    xdcolo->setHorizontalHeaderLabels(QStringList() << tr("Frame") << tr("TID") << tr("SID"));
    ui->tableViewXDColo->setModel(xdcolo);
    ui->tableViewXDColo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    frlge->setHorizontalHeaderLabels(QStringList() << tr("Frame") << tr("TID") << tr("SID"));
    ui->tableViewFRLGE->setModel(frlge);
    ui->tableViewFRLGE->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    rs->setHorizontalHeaderLabels(QStringList() << tr("Frame") << tr("TID") << tr("SID"));
    ui->tableViewRS->setModel(rs);
    ui->tableViewRS->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void IDs3::on_pushButtonFindFRLGE_clicked()
{
    frlge->removeRows(0, frlge->rowCount());

    u16 tid = ui->textBoxTIDFRLGE->text().toUShort();
    bool usePID = ui->checkBoxPIDFRLGE->isChecked();
    bool useSID = ui->checkBoxSIDFRLGE->isChecked();
    u32 pid = ui->textBoxPIDFRLGE->text().toUInt(nullptr, 16);
    u16 searchSID = ui->textBoxSIDFRLGE->text().toUShort();
    u32 minFrame = ui->textBoxMinFrameFRLGE->text().toUInt();
    u32 maxResults = ui->textBoxMaxFrameFRLGE->text().toUInt();

    PokeRNG rng(tid, minFrame - 1);
    u16 sid = rng.nextUShort();

    u32 max = minFrame + maxResults;
    for (u32 frame = minFrame; frame <= max; ++frame)
    {
        sid = rng.nextUShort();

        if ((!usePID || Utilities::shiny(pid, tid, sid)) && (!useSID || searchSID == sid))
        {
            frlge->appendRow(QList<QStandardItem *>() << new QStandardItem(QString::number(frame)) << new QStandardItem(QString::number(tid)) << new QStandardItem(QString::number(sid)));
        }
    }
}

void IDs3::on_pushButtonFindRS_clicked()
{
    rs->removeRows(0, rs->rowCount());

    u32 seed;
    bool usePID = ui->checkBoxPIDRS->isChecked();
    bool useSID = ui->checkBoxSIDRS->isChecked();
    bool useTID = ui->checkBoxTIDRS->isChecked();
    u32 pid = ui->textBoxPIDRS->text().toUInt(nullptr, 16);
    u16 searchSID = ui->textBoxSIDRS->text().toUShort();
    u16 searchTID = ui->textBoxTIDRS->text().toUShort();
    u32 minFrame = ui->textBoxMinFrameRS->text().toUInt();
    u32 maxResults = ui->textBoxMaxFrameRS->text().toUInt();

    if (ui->radioButtonInitSeedRS->isChecked())
    {
        seed = ui->textBoxInitSeedRS->text().toUInt(nullptr, 16);
    }
    else
    {
        seed = Utilities::calcGen3Seed(ui->dateTimeEdit->date(), ui->dateTimeEdit->time().hour(), ui->dateTimeEdit->time().minute());
    }

    PokeRNG rng(seed, minFrame);

    u16 tid = rng.nextUShort(), sid;

    u32 max = minFrame + maxResults;
    for (u32 frame = minFrame; frame <= max; ++frame)
    {
        sid = tid;
        tid = rng.nextUShort();

        if ((!usePID || Utilities::shiny(pid, tid, sid)) && (!useTID || searchTID == tid) && (!useSID || searchSID == sid))
        {
            rs->appendRow(QList<QStandardItem *>() << new QStandardItem(QString::number(frame)) << new QStandardItem(QString::number(tid)) << new QStandardItem(QString::number(sid)));
        }
    }
}

void IDs3::on_pushButtonFindXD_clicked()
{
    xdcolo->removeRows(0, xdcolo->rowCount());

    u32 seed = ui->textBoxSeedXD->text().toUInt(nullptr, 16);
    bool usePID = ui->checkBoxPIDXD->isChecked();
    bool useSID = ui->checkBoxSIDXD->isChecked();
    bool useTID = ui->checkBoxTIDXD->isChecked();
    u32 pid = ui->textBoxPIDXD->text().toUInt(nullptr, 16);
    u16 searchSID = ui->textBoxSIDXD->text().toUShort();
    u16 searchTID = ui->textBoxTIDXD->text().toUShort();
    u32 minFrame = ui->textBoxMinFrameXD->text().toUInt();
    u32 maxResults = ui->textBoxMaxFrameXD->text().toUInt();

    XDRNG rng(seed, minFrame + 1);
    u16 sid = rng.nextUShort(), tid;

    u32 max = minFrame + maxResults;
    for (u32 frame = minFrame; frame <= max; ++frame)
    {
        tid = sid;
        sid = rng.nextUShort();

        if ((!usePID || Utilities::shiny(pid, tid, sid)) && (!useTID || searchTID == tid) && (!useSID || searchSID == sid))
        {
            xdcolo->appendRow(QList<QStandardItem *>() << new QStandardItem(QString::number(frame)) << new QStandardItem(QString::number(tid)) << new QStandardItem(QString::number(sid)));
        }
    }
}

void IDs3::on_checkBoxPIDXD_stateChanged(int val)
{
    ui->textBoxPIDXD->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxTIDXD_stateChanged(int val)
{
    ui->textBoxTIDXD->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxSIDXD_stateChanged(int val)
{
    ui->textBoxSIDXD->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxPIDFRLGE_stateChanged(int val)
{
    ui->textBoxPIDFRLGE->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxSIDFRLGE_stateChanged(int val)
{
    ui->checkBoxSIDFRLGE->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxBattery_stateChanged(int val)
{
    bool flag = val == Qt::Unchecked;
    ui->radioButtonDateRS->setEnabled(flag);
    ui->radioButtonInitSeedRS->setEnabled(flag);
    ui->dateTimeEdit->setEnabled(flag ? ui->radioButtonDateRS->isChecked() : false);
    ui->textBoxInitSeedRS->setEnabled(flag ? ui->radioButtonInitSeedRS->isChecked() : false);
}

void IDs3::on_checkBoxPIDRS_stateChanged(int val)
{
    ui->textBoxPIDRS->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxTIDRS_stateChanged(int val)
{
    ui->textBoxTIDRS->setEnabled(val == Qt::Checked);
}

void IDs3::on_checkBoxSIDRS_stateChanged(int val)
{
    ui->textBoxSIDRS->setEnabled(val == Qt::Checked);
}

void IDs3::on_radioButtonDateRS_toggled(bool checked)
{
    ui->dateTimeEdit->setEnabled(checked);
}

void IDs3::on_radioButtonInitSeedRS_toggled(bool checked)
{
    ui->textBoxInitSeedRS->setEnabled(checked);
}
