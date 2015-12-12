// SampleApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\ViewItemFactory.h"
#include "..\..\DefaultViewItem.h"
#include "..\..\DefaultViewItemCreator.h"
#include "..\..\FloatViewItemCreator.h"
#include "..\..\RTValViewItemCreator.h"
#include "..\..\Vec3ViewItemCreator.h"
#include "SampleModel.h"

static QTime myTimer;
void SetStylesheet(const char* filename)
{
	QFile File(filename);
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());

	qApp->setStyleSheet(StyleSheet);
}

class MainWindow : public QMainWindow
{
  void BuildTree( QTreeWidget* tree, QTreeWidgetItem* parent, BaseViewItem* pViewItem )
  {
    if (pViewItem == nullptr)
      return;

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText( 0, pViewItem->GetName() );

    if (parent == nullptr)
      tree->addTopLevelItem( item );
    else
      parent->addChild( item );

    QWidget* viewWidget = pViewItem->BuildWidgets( false );
    tree->setItemWidget( item, 1, viewWidget );

    for (int i = 0; i < pViewItem->NumChildren(); i++)
    {
      BuildTree( tree, item, pViewItem->GetChild( i ) );
    }

    // Once the children are added/built, update values from source
    pViewItem->UpdateValueFromModel();
  }

public:
	MainWindow(BaseViewItem* pBaseView)
	{

		QTreeWidget* tree = new QTreeWidget;
		tree->setColumnCount(2);

		BuildTree(tree, nullptr, pBaseView);

		//for (int i = 0; i < 1000; ++i) {
		//	QTreeWidgetItem* item = new QTreeWidgetItem;
		//	item->setText(0, QString::number(i));
		//	tree->addTopLevelItem(item);

		//	//create a layout with parent
		//	QWidget* w = new QWidget;
		//	QHBoxLayout *layout = new QHBoxLayout(w);
		//	//widgets that will be contained in QWidget
		//	QLabel* label1 = new QLabel("Label 1");
		//	QLabel* label2 = new QLabel("Label 2");
		//	layout->addWidget(label1); //add contained widgets to layout
		//	layout->addWidget(label2);
		//	layout->addWidget(new QSpinBox());


		//	tree->setItemWidget(item, 1, w);


		//}
		setCentralWidget(tree);
	}

	virtual void showEvent(QShowEvent *ev) override
	{
		__super::showEvent(ev);
		int elapsed = myTimer.elapsed();
		printf("elapsed: %i\n", elapsed);
	}
};


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//SetStylesheet("styles.qss");

	BaseModelItem* pSampleModel = BuildSampleModel();
	
	ViewItemFactory* pFactory = ViewItemFactory::GetInstance();
	
	// Register different adaptor types
	DefaultViewItemCreator defCreator;
	FloatViewItemCreator fltCreator;
	Vec3ViewItemCreator vec3Creator;
	// RTValViewItemCreator rtCreator;

	BaseViewItem* pViewLayer = pFactory->BuildView(pSampleModel);


	myTimer.start();
	MainWindow w(pViewLayer);

	// Now everything is connected up, try modifying a model value
	BaseModelItem* pAModel = pSampleModel->GetChild(1);
	SampleModelItem* pFloatModel = dynamic_cast<SampleModelItem*>(pAModel);
	if (pFloatModel)
	{
		QVariant anotherValue(45.43f);
		pFloatModel->SetValueDirect(anotherValue);
	}
	w.show();
	return a.exec();
}