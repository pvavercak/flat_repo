#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QString>
#include <QtTest>
#include <clientgui.h>

const QString STR1("blog.");
const QString STR2("davidecoppola.com");
const QString STR_RES(STR1 + STR2);

class TestClientGUI : public QObject
{
	Q_OBJECT

private slots:
	void init();

	void TestSignals();

private:
    Ui::ClientGUI m_clientgui;
};

void TestClientGUI::init()
{
    //m_clientgui.CancelData();
}



void TestClientGUI::TestSignals()
{
	// set input
    m_clientgui.ui->ipaddr->setText("515151115");

	// create spy objects
    QSignalSpy spy1(&m_clientgui, &ClientGUI::on_send_pressed);
    QSignalSpy spy2(&m_clientgui, &ClientGUI::attemptConnection);

	// click button CONCAT
    QTest::mouseClick(m_clientgui.ui->sendbutton, Qt::LeftButton);

	QCOMPARE(spy1.count(), 1);
	QCOMPARE(spy2.count(), 0);

//	QList<QVariant> args = spy1.takeFirst();
//	QCOMPARE(args.at(0).toString(), STR_RES);
//	args = spy2.takeFirst();
//	QVERIFY2(args.empty(), "DataCleared signal has parameters now?!?");
}

QTEST_MAIN(TestClientGUI)
