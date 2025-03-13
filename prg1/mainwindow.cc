// The main program (provided by the course), COMP.CS.300
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include "mainprogram.hh"

// Qt generated main window code

#ifdef GRAPHICAL_GUI
#include <QCoreApplication>
#include <QFileDialog>
#include <QDir>
#include <QFont>
#include <QGraphicsItem>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QVariant>

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <iostream>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <tuple>

#include <cassert>

#include "mainwindow.hh"
#include "ui_mainwindow.h"

// Needed to be able to store PlaceID in QVariant (in QGraphicsItem)
Q_DECLARE_METATYPE(PlaceID)
// The same for AreaIDs (currently an unsigned long long int)
//struct AreaIDcont { AreaID id; }; // So that we have a different type than for Place IDs
Q_DECLARE_METATYPE(AreaID)
// The same for Coords (currently a pair of ints)
Q_DECLARE_METATYPE(Coord)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainprg_.setui(this);

    // Execute line
    connect(ui->execute_button, &QPushButton::pressed, this, &MainWindow::execute_line);

    // Line edit
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::execute_line);

    // File selection
    connect(ui->file_button, &QPushButton::pressed, this, &MainWindow::select_file);

    // Command selection
    // !!!!! Sort commands in alphabetical order (should not be done here, but is)
    std::sort(mainprg_.cmds_.begin(), mainprg_.cmds_.end(), [](auto const& l, auto const& r){ return l.cmd < r.cmd; });
    for (auto& cmd : mainprg_.cmds_)
    {
        ui->cmd_select->addItem(QString::fromStdString(cmd.cmd));
    }
    connect(ui->cmd_select, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::cmd_selected);

    // Number selection
    for (auto i = 0; i <= 20; ++i)
    {
        ui->number_select->addItem(QString("%1").arg(i));
    }
    connect(ui->number_select, static_cast<void(QComboBox::*)(QString const&)>(&QComboBox::currentTextChanged), this, &MainWindow::number_selected);

    // Output box
    QFont monofont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monofont.setStyleHint(QFont::TypeWriter);
    ui->output->setFont(monofont);

    // Initialize graphics scene & view
    gscene_ = new QGraphicsScene(this);
    ui->graphics_view->setScene(gscene_);
    ui->graphics_view->resetTransform();
    ui->graphics_view->setBackgroundBrush(Qt::black);

    // Selecting graphics items by mouse
    connect(gscene_, &QGraphicsScene::selectionChanged, this, &MainWindow::scene_selection_change);
//    connect(this, &MainProgram::signal_clear_selection, this, &MainProgram::clear_selection);

    // Zoom slider changes graphics view scale
    connect(ui->zoom_plus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1.1, 1.1); });
    connect(ui->zoom_minus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1/1.1, 1/1.1); });
    connect(ui->zoom_1, &QToolButton::clicked, [this]{ this->ui->graphics_view->resetTransform(); });
    connect(ui->zoom_fit, &QToolButton::clicked, this, &MainWindow::fit_view);

    // Changing checkboxes updates view
    connect(ui->places_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->placenames_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->areas_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->ways_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);

    // Unchecking places checkbox disables place names checkbox
    connect(ui->places_checkbox, &QCheckBox::clicked,
            [this]{ this->ui->placenames_checkbox->setEnabled(this->ui->places_checkbox->isChecked()); });

    // Changing font or points scale updates view
    connect(ui->fontscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);
    connect(ui->pointscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);

    // Clear input button
    connect(ui->clear_input_button, &QPushButton::clicked, this, &MainWindow::clear_input_line);

    // Stop button
    connect(ui->stop_button, &QPushButton::clicked, [this](){ this->stop_pressed_ = true; });

    clear_input_line();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_view()
{
    std::unordered_set<std::string> errorset;
    try
    {
        gscene_->clear();
        auto pointscale = ui->pointscale->value();
        auto fontscale = ui->fontscale->value();

        // Collect the result of previous operation
        std::unordered_map<PlaceID, std::string> result_places;
        std::unordered_map<AreaID, std::string> result_areas;
        switch (mainprg_.prev_result.first)
        {
        case MainProgram::ResultType::IDLIST:
            {
                // Copy the id vectors to the result maps
                auto& prev_result = std::get<MainProgram::CmdResultIDs>(mainprg_.prev_result.second);
                int i = 0;
                std::for_each(prev_result.second.begin(), prev_result.second.end(),
                              [&result_places, &i](auto id){ result_places[id] += MainProgram::convert_to_string(++i)+". "; });
                i = 0;
                std::for_each(prev_result.first.begin(), prev_result.first.end(),
                              [&result_areas, &i](auto id){ result_areas[id] += MainProgram::convert_to_string(++i)+". "; });
            }
            break;
        case MainProgram::ResultType::NOTHING:
            break;
        case MainProgram::ResultType::NAMELIST:
            break;
        case MainProgram::ResultType::TYPELIST:
            break;
        case MainProgram::ResultType::COORDLIST:
            break;
        default:
            assert(!"Unhandled result type in update_view()!");
        }

        auto places = mainprg_.ds_.all_places();
        if (places.size() == 1 && places.front() == NO_PLACE)
        {
            errorset.insert("all_places() returned error {NO_PLACE}");
        }

        if (ui->places_checkbox->isChecked())
        {
            auto places = mainprg_.ds_.all_places();
            if (places.size() == 1 && places.front() == NO_PLACE)
            {
                errorset.insert("Error from GUI: all_places() returned error {NO_PLACE}");
                places.clear(); // Clear the places so that no more errors are caused by NO_PLACE
            }

            for (auto placeid : places)
            {
                QColor placecolor = Qt::gray;
                QColor namecolor = Qt::cyan;
                QColor placeborder = Qt::gray;
                int placezvalue = 1;

                try
                {
                    if (placeid != NO_PLACE)
                    {
                        auto xy = mainprg_.ds_.get_place_coord(placeid);
                        auto [x,y] = xy;
                        if (x == NO_VALUE || y == NO_VALUE)
                        {
                            errorset.insert("get_place_coord() returned error NO_COORD/NO_VALUE");
                        }

                        if (x == NO_VALUE || y == NO_VALUE)
                        {
                            x = 0; y = 0;
                            placecolor = Qt::magenta;
                            namecolor = Qt::magenta;
                            placezvalue = 30;
                        }

                        string prefix;
                        auto res_place = result_places.find(placeid);
                        if (res_place != result_places.end())
                        {
                            if (result_places.size() > 1) { prefix = res_place->second; }
                            namecolor = Qt::red;
                            placeborder = Qt::red;
                            placezvalue = 2;
                        }

                        if (ui->places_checkbox->isChecked())
                        {
                            auto groupitem = gscene_->createItemGroup({});
                            groupitem->setFlag(QGraphicsItem::ItemIsSelectable);
                            groupitem->setData(0, QVariant::fromValue(placeid));

                            QPen placepen(placeborder);
                            placepen.setWidth(0); // Cosmetic pen
                            auto dotitem = gscene_->addEllipse(-4*pointscale, -4*pointscale, 8*pointscale, 8*pointscale,
                                                               placepen, QBrush(placecolor));
                            dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                            groupitem->addToGroup(dotitem);
                            //        dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                            //        dotitem->setData(0, QVariant::fromValue(town));

                            // Draw place names
                            string label = prefix;
                            if (ui->placenames_checkbox->isChecked())
                            {
                                try
                                {
                                    auto name = mainprg_.ds_.get_place_name(placeid);
                                    if (name == NO_NAME)
                                    {
                                        errorset.insert("get_place_name() returned error NO_NAME");
                                    }

                                    label += name;
                                }
                                catch (NotImplemented const& e)
                                {
                                    errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                                    std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                                }
                            }

                            if (!label.empty())
                            {
                                // Create extra item group to be able to set ItemIgnoresTransformations on the correct level (addSimpleText does not allow
                                // setting initial coordinates in item coordinates
                                auto textgroupitem = gscene_->createItemGroup({});
                                auto textitem = gscene_->addSimpleText(QString::fromStdString(label));
                                auto font = textitem->font();
                                font.setPointSizeF(font.pointSizeF()*fontscale);
                                textitem->setFont(font);
                                textitem->setBrush(QBrush(namecolor));
                                textitem->setPos(-textitem->boundingRect().width()/2, -4*pointscale - textitem->boundingRect().height());
                                textgroupitem->addToGroup(textitem);
                                textgroupitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                                groupitem->addToGroup(textgroupitem);
                            }

                            groupitem->setPos(20*x, -20*y);
                            groupitem->setZValue(placezvalue);
                        }
                    }
                }
                catch (NotImplemented const& e)
                {
                    errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                    std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                }
            }
        }

        // Draw areas
        if (ui->areas_checkbox->isChecked())
        {
            try
            {
                auto areaids = mainprg_.ds_.all_areas();
                if (areaids.size() == 1 && areaids.front() == NO_AREA)
                {
                    errorset.insert("all_areas() returned error {NO_AREA}");
                }

                for (auto areaid : areaids)
                {
                    QColor areacolor = Qt::blue;
                    int areazvalue = -3;

                    try
                    {
                        if (areaid != NO_AREA)
                        {
                            if (result_areas.find(areaid) != result_areas.end())
                            {
                                areacolor = Qt::green;
                                areazvalue = -2;
                            }
                            auto coords = mainprg_.ds_.get_area_coords(areaid);
                            if (coords.size() < 3)
                            {
                                errorset.insert("get_area_coordinates() returned too few coordinates (under 3)");
                            }
                            else if (std::find(coords.begin(), coords.end(), NO_COORD) != coords.end())
                            {
                                errorset.insert("get_area_coordinates() returned error NO_COORD");
                            }
                            else
                            {
                                auto pen = QPen(areacolor);
                                pen.setWidth(0); // "Cosmetic" pen
                                Coord prevcoord = NO_COORD;
                                for (auto& coord : coords)
                                {
                                    if (prevcoord != NO_COORD)
                                    {
                                        QLineF line(QPointF(20*prevcoord.x, -20*prevcoord.y), QPointF(20*coord.x, -20*coord.y));
                                        auto lineitem = gscene_->addLine(line, pen);
                                        lineitem->setFlag(QGraphicsItem::ItemIsSelectable);
                                        lineitem->setData(0, QVariant::fromValue(areaid));
                                        lineitem->setZValue(areazvalue);
                                    }
                                    prevcoord = coord;
                                }
                                // Close the loop
                                QLineF line(QPointF(20*prevcoord.x, -20*prevcoord.y), QPointF(20*coords.front().x, -20*coords.front().y));
                                auto lineitem = gscene_->addLine(line, pen);
                                lineitem->setFlag(QGraphicsItem::ItemIsSelectable);
                                lineitem->setData(0, QVariant::fromValue(areaid));
                                lineitem->setZValue(areazvalue);
                            }
                        }
                    }
                    catch (NotImplemented const& e)
                    {
                        errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                        std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                    }
                }
            }
            catch (NotImplemented const& e)
            {
                errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
            }
        }
    }
    catch (NotImplemented const& e)
    {
        errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
        std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
    }

    if (!errorset.empty())
    {
        std::ostringstream errorstream;
        for (auto const& errormsg : errorset)
        {
            errorstream << "Error from GUI: " << errormsg << std::endl;
        }
        output_text(errorstream);
        output_text_end();
    }
}

void MainWindow::output_text(ostringstream& output)
{
    string outstr = output.str();
    if (!outstr.empty())
    {
        if (outstr.back() == '\n') { outstr.pop_back(); } // Remove trailing newline
        ui->output->appendPlainText(QString::fromStdString(outstr));
        ui->output->ensureCursorVisible();
        ui->output->repaint();
    }

    output.str(""); // Clear the stream, because it has already been output
}

void MainWindow::output_text_end()
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->ensureCursorVisible();
    ui->output->repaint();
}

bool MainWindow::check_stop_pressed() const
{
    QCoreApplication::processEvents();
    return stop_pressed_;
}

void MainWindow::execute_line()
{
    auto line = ui->lineEdit->text();
    clear_input_line();
    ui->output->appendPlainText(QString::fromStdString(MainProgram::PROMPT)+line);

    ui->execute_button->setEnabled(false);
    ui->stop_button->setEnabled(true);
    stop_pressed_ = false;

    ostringstream output;
    bool cont = mainprg_.command_parse_line(line.toStdString(), output);
    ui->lineEdit->clear();
    output_text(output);
    output_text_end();

    ui->stop_button->setEnabled(false);
    ui->execute_button->setEnabled(true);
    stop_pressed_ = false;

    ui->lineEdit->setFocus();

//    if (mainprg_.view_dirty)
//    {
//        update_view();
//        mainprg_.view_dirty = false;
//    }
    update_view();

    if (!cont)
    {
        close();
    }
}

void MainWindow::cmd_selected(int idx)
{
    ui->lineEdit->insert(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "));
    ui->cmd_info_text->setText(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "+mainprg_.cmds_[idx].info));

    ui->lineEdit->setFocus();
}

void MainWindow::number_selected(QString const& number)
{
    ui->lineEdit->insert(number+" ");

    ui->lineEdit->setFocus();
}

void MainWindow::select_file()
{
    QFileDialog dialog(this, "Select file", QDir::currentPath(), "Command list (*.txt)");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec())
    {
        auto filenames = dialog.selectedFiles();
        for (auto& i : filenames)
        {
            auto filename = QDir("").relativeFilePath(i);
            ui->lineEdit->insert("\""+filename+"\" ");
        }
    }

    ui->lineEdit->setFocus();
}

void MainWindow::clear_input_line()
{
    ui->cmd_info_text->clear();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void MainWindow::fit_view()
{
    ui->graphics_view->fitInView(gscene_->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::scene_selection_change()
{
    auto items = gscene_->selectedItems();
    if (!items.empty())
    {
        for (auto i : items)
        {
            auto data = i->data(0);
//            assert(stopid!=NO_ID && "NO_ID as stop in graphics view");
            if (!selection_clear_in_progress)
            {
                ostringstream output;
                output << "*click* ";
                std::string ui_str;
                if (data.canConvert<PlaceID>())
                {
                    auto placeid = data.value<PlaceID>();
                    ui_str = mainprg_.print_place(placeid, output);
                }
                if (data.canConvert<AreaID>())
                {
                    auto areaid = data.value<AreaID>();
                    ui_str = mainprg_.print_area(areaid, output);
                }
                else if (data.canConvert<Coord>())
                {
                    auto coord = data.value<Coord>();
                    ui_str = mainprg_.print_coord(coord, output);
                }
                if (!ui_str.empty()) { ui->lineEdit->insert(QString::fromStdString(ui_str+" ")); }
                output_text(output);
                output_text_end();
            }
            i->setSelected(false);
            selection_clear_in_progress = !selection_clear_in_progress;
        }
    }
}

void MainWindow::clear_selection()
{
    gscene_->clearSelection();
}



// Originally in main.cc
#include <QApplication>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        auto status = a.exec();
        std::cerr << "Program ended normally." << std::endl;
        return status;
    }
    else
    {
        return MainProgram::mainprogram(argc, argv);
    }
}

#else

int main(int argc, char *argv[])
{
    return MainProgram::mainprogram(argc, argv);
}
#endif
