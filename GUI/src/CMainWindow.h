#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H
#include <boost/asio.hpp>
#include <QtWidgets>
#include <QMainWindow>
#include <CPaintWidget.h>
#include <QListWidget>
#include <QToolBar>
#include <CThread.h>
#include <CSettingDlg.h>
#include <CStepDlg.h>


class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  CMainWindow();
  void print(int p,int q);

private slots:
  void snapAPicture();
  
  /**@brief setting data
   */
  void setting();

  /** @brief save data into disk
   */
  void saveData();

  /** @brief load data into disk
   */
  void loadData();

  /** @brief step running
   */
  void step();

protected:
  void closeEvent(QCloseEvent* event);

private:
  void createActions();
  void createToolBars();
  void createStatusBar();
  
  //
  CPaintWidget *m_pPaint;
  QListWidget  *m_pList;
  QToolBar     *m_pEditToolBar;
  QToolBar     *m_pFileToolBar;
  CThread      *m_pThread;
  CSettingDlg  *m_pSettingDlg;
  CStepDlg     *m_pStepDlg;

  
  // actions
  QAction *m_pSnapAPictureAction;
  QAction *m_pStartProcessAction;
  QAction *m_pStopProcessAction;
  QAction *m_pSettingAction;
  QAction *m_pSaveDataAction;
  QAction *m_pLoadDataAction;
  QAction *m_pStepAction;

};
#endif //CMAINWIDONW_H
