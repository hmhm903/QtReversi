#include <QApplication>

#include "reversi.h"
#include "manager.h"


int main(int argc, char *argv[])
{
	//Mode::Multi is optional setting for developer. It is not support the restart operation.
	Mode mode = Mode::Default;
	QApplication a(argc, argv);

	Manager manager;
	unsigned int i_num = 1;
	unsigned int j_num = 1;
	if (mode == Mode::Multi) {
		i_num = 4;
		j_num = 4;
	}
	
	for(unsigned int i = 0; i != i_num; i++)
	{
		for(unsigned int j =0; j != j_num; j++)
		{
			Reversi* reversi = new Reversi(mode, PlayerType::Human, PlayerType::Computer);
			Vec2d render_pos(i * 200, j * 200);
			manager.w->AddReversi(reversi, render_pos);
			manager.reversi_list_.push_back(reversi);
		}
	}

	for (unsigned int i = 0; i != manager.reversi_list_.size(); i++)
	{
		QObject::connect(manager.reversi_list_[i], SIGNAL(finishedPostProcessSignal(unsigned int)),
			manager.w, SLOT(finishedPostProcessSlot(unsigned int)));

		QObject::connect(manager.w, SIGNAL(nextPreProcessSignal(unsigned int)),
			&manager, SLOT(nextPreprocessSlot(unsigned int)));

		QObject::connect(manager.reversi_list_[i], SIGNAL(repaintSignal(unsigned int)),
			manager.w, SLOT(repaintSlot(unsigned int)));
	}
	if (manager.reversi_list_.size() == 1)
	{
		QObject::connect(manager.w, SIGNAL(leftClickSignal(pair<unsigned int, unsigned int>)),
			manager.reversi_list_[0], SLOT(leftClickSlot(pair<unsigned int, unsigned int>)));
	}
    QObject::connect(manager.w, SIGNAL(restartSignal()), &manager, SLOT(restartSlot()));

	manager.reversi_list_[0]->PreProcess();

    return a.exec();
}
