#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qevent.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QString>

#include "cell.h"
#include "board.h"
#include "reversi.h"
#include "player.h"


MainWindow::MainWindow(QWidget *parent)
	:kWindowWidth(700), kWindowHeight(700), QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	this->resize(kWindowWidth, kWindowHeight);

	scene_ = new QGraphicsScene(QRect(0, 0, kWindowWidth, kWindowHeight));
	view_ = new QGraphicsView(scene_);
	view_->setBackgroundBrush(QBrush(Qt::gray));
	setCentralWidget(view_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddReversi(Reversi * reversi, Vec2d render_pos)
{
	pair<unsigned int, unsigned int>  size = reversi->GetBoardPtr()->GetBoardSize();
	Vec2d render_board_size(kCellSize * size.first, kCellSize * size.second);

	RenderReversi render_reversi(reversi, render_pos, render_board_size);

	render_reversi_list_.push_back(render_reversi);
	return;
}

void MainWindow::finishedTimerSlot()
{
	emit nextPreProcessSignal();
}

void MainWindow::repaintSlot()
{
	this->repaint();
}

void MainWindow::finishedPostProcessSlot()
{
	this->repaint();
	QTimer::singleShot(1, this, SLOT(finishedTimerSlot()));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
//	top_left_ = Vec2d((kWindowWidth - kSceneWidth) / 2, (kWindowHeight - kSceneHeight) / 2);


	//if (event->button() == Qt::LeftButton) {
	//	int x = event->x() - top_left_.first;
	//	int y = event->y() - top_left_.second;
	//	Vec2d put_pos(x / kCellSize, y / kCellSize);

	//	// modify put position. ex: (0,0)->(1,1)
	//	put_pos.first += 1;
	//	put_pos.second += 1;

	//	if(Reversi::GetWaitingFlag())
	//		emit leftClickSignal(put_pos);
	//	this->repaint();
	//}
}

void MainWindow::PaintOutline(pair<unsigned int, unsigned int> board_size, Vec2d render_offset)
{
	const int kBoardWidth = kCellSize * board_size.first;
	const int kBoardHeight = kCellSize * board_size.second;

	QPen pen(Qt::black, 2);
	for (unsigned int x = 0; x <= board_size.first; x++)
	{
		QPoint start(x * kCellSize + render_offset.first, render_offset.second);
		QPoint end(x * kCellSize + render_offset.first, kBoardWidth + render_offset.second);
		QLine line(start, end);
		scene_->addLine(line, pen);
	}

	for (unsigned int y = 0; y <= board_size.first; y++)
	{
		QPoint start(0+ render_offset.first, y * kCellSize + render_offset.second);
		QPoint end(kBoardHeight + render_offset.first, y * kCellSize + render_offset.second);
		QLine line(start, end);
		scene_->addLine(line, pen);
	}
	return;
}

void MainWindow::PaintBoard(Board* board, Vec2d render_offset)
{
	Vec2d size = board->GetBoardSize();

	// render board
	const unsigned int kBoardWidth = kCellSize * 8;
	const unsigned int kBoardHeight = kCellSize * 8;
	scene_->addRect(render_offset.first, render_offset.second, kBoardWidth, kBoardHeight, QPen(Qt::black), QBrush(Qt::darkGreen));

	// render outline
	PaintOutline(size, render_offset);

	// render stone
	for (unsigned int y = 0; y != size.first + 2; y++)
	{
		for (unsigned int x = 0; x != size.second + 2; x++)
		{
			Vec2d pos(x, y);
			Cell cell = board->GetCell(pos);
			CELL_STATE state = cell.GetCellState();
			switch (state)
			{
			case CELL_STATE::EMPTY:
			case CELL_STATE::AROUND:
				break;

			case CELL_STATE::STONE:
				switch (cell.GetStoneColor())
				{
				case STONE_COLOR::BLACK:
					PaintStone(pos, STONE_COLOR::BLACK, render_offset);
					break;

				case STONE_COLOR::WHITE:
					PaintStone(pos, STONE_COLOR::WHITE, render_offset);
					break;

				default:
					break;
				}

				break;
			default:
				break;
			}
		}
	}
}

void MainWindow::PaintPlayerInfo(Player * player, Vec2d render_offset)
{
	STONE_COLOR color = player->GetPlayerColor();
	QString print_str;
	switch (color)
	{
	case STONE_COLOR::BLACK:
		print_str = "Black";
		break;
	case STONE_COLOR::WHITE:
		print_str = "White";
		break;
	default:
		break;
	}
	if(player->IsPass())
		print_str = "Pass";

	const unsigned int kBoardWidth = kCellSize * 8;
	const unsigned int kBoardHeight = kCellSize * 8;
	QFont font("Times", 20, QFont::Bold);
	QGraphicsTextItem* text = scene_->addText(print_str, font);
	text->setPos(render_offset.first, kBoardHeight + render_offset.second);
	text->deleteLater();

}

void MainWindow::PaintGameResult(JudgeResult result, Vec2d render_offset)
{
	QString print_str;
	switch (result)
	{
	case JudgeResult::BlackWin:
		print_str = "Black Win!";
		break;
	case JudgeResult::WhiteWin:
		print_str = "White Win!";
		break;
	case JudgeResult::Draw:
		print_str = "Draw";
		break;
	default:
		break;
	}

	const unsigned int kBoardWidth = kCellSize * 8;
	const unsigned int kBoardHeight = kCellSize * 8;
	QFont font("Times", 20, QFont::Bold);
	QGraphicsTextItem* text = scene_->addText(print_str, font);
	text->setPos(kBoardWidth + render_offset.first - 200, kBoardHeight + render_offset.second);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
	for (auto render_reversi : render_reversi_list_)
	{
		PaintBoard(render_reversi.reversi->GetBoardPtr(), render_reversi.render_pos);
		PaintPlayerInfo(render_reversi.reversi->GetNowPlayer(), render_reversi.render_pos);
		JudgeResult result = render_reversi.reversi->GetGameResult();
		if (result != JudgeResult::NotFinished)
			PaintGameResult(result, render_reversi.render_pos);
	}
	return;
}
