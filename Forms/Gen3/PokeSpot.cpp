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

#include "PokeSpot.hpp"
#include "ui_PokeSpot.h"

PokeSpot::PokeSpot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PokeSpot)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    setupModels();
}

PokeSpot::~PokeSpot()
{
    saveSettings();

    delete ui;
    delete model;
}

void PokeSpot::changeEvent(QEvent *event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }
}

void PokeSpot::on_pushButtonAnyAbility_clicked()
{
    ui->comboBoxAbility->setCurrentIndex(0);
}

void PokeSpot::on_pushButtonAnyNature_clicked()
{
    ui->comboBoxNature->uncheckAll();
}

void PokeSpot::on_pushButtonAnySpotType_clicked()
{
    ui->comboBoxSpotType->uncheckAll();
}

void PokeSpot::setupModels()
{
    ui->textBoxSeed->setValues(InputType::Seed32Bit);
    ui->textBoxStartingFrame->setValues(InputType::Frame32Bit);
    ui->textBoxMaxResults->setValues(InputType::Frame32Bit);
    ui->textBoxTID->setValues(InputType::TIDSID);
    ui->textBoxSID->setValues(InputType::TIDSID);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setModel(model);

    ui->comboBoxNature->setup();
    ui->comboBoxSpotType->setup();

    loadSettings();
}

void PokeSpot::saveSettings()
{
    QSettings setting;
    setting.setValue("pokespotTID", ui->textBoxTID->text());
    setting.setValue("pokespotSID", ui->textBoxSID->text());
}

void PokeSpot::loadSettings()
{
    QSettings setting;
    if (setting.contains("pokespotTID")) ui->textBoxTID->setText(setting.value("pokespotTID").toString());
    if (setting.contains("pokespotSID")) ui->textBoxSID->setText(setting.value("pokespotSID").toString());
}

void PokeSpot::on_pushButtonGenerate_clicked()
{
    model->clear();

    QVector<Frame3> frames;

    u32 seed = ui->textBoxSeed->text().toUInt(nullptr, 16);
    u32 initialFrame = ui->textBoxStartingFrame->text().toUInt();
    u32 maxResults = ui->textBoxMaxResults->text().toUInt();
    u16 tid = ui->textBoxTID->text().toUShort();
    u16 sid = ui->textBoxSID->text().toUShort();

    int genderRatio = ui->comboBoxGenderRatio->currentIndex();

    XDRNG rng(seed, initialFrame - 1);
    u16 *rngArray = new u16[maxResults + 5];
    for (u32 x = 0; x < maxResults + 5; x++)
        rngArray[x] = rng.nextUShort();

    u32 call1, call2, call3;

    Frame3 frame = Frame3(tid, sid, tid ^ sid);
    FrameCompare compare = FrameCompare(ui->comboBoxGender->currentIndex(), genderRatio, ui->comboBoxAbility->currentIndex(),
                                        ui->comboBoxNature->getChecked(), ui->checkBoxShinyOnly->isChecked());
    frame.setGenderRatio(static_cast<u32>(genderRatio));

    QVector<bool> spots = ui->comboBoxSpotType->getChecked();

    for (u32 cnt = 0; cnt < maxResults; cnt++)
    {
        // Check if frame is a valid pokespot call
        call1 = rngArray[cnt] % 3;
        if (call1 != 0)
            continue;

        // Munchlax provides a frame skip
        call2 = rngArray[1 + cnt] % 100;
        if (call2 < 10)
            continue;

        // Check what type the pokespot is
        call3 = rngArray[2 + cnt] % 100;
        if (call3 < 50)
        {
            if (!spots[0])
                continue;
            frame.setLockReason(tr("Common"));
        }
        else if (call3 > 49 && call3 < 85)
        {
            if (!spots[1])
                continue;
            frame.setLockReason(tr("Uncommon"));
        }
        else
        {
            if (!spots[2])
                continue;
            frame.setLockReason(tr("Rare"));
        }

        frame.setPID(rngArray[4 + cnt], rngArray[3 + cnt]);
        if (compare.comparePID(frame))
        {
            frame.setFrame(cnt);
            frames.append(frame);
        }
    }
    model->setModel(frames);
}
