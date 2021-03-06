
#include <CPaintWidget.h>
#include <Camera/CCamera.h>
#include <misc/CDataSet.h>

#include <sstream>
#include <iostream>
using std::cout;
using std::endl;
using std::ostringstream;


CPaintWidget::CPaintWidget(QListWidget* pList,QWidget* parent/*=0*/)
  : m_pList(pList),
    QWidget(parent)
{
  connect(this, SIGNAL(refresh()),
	  this, SLOT(refreshPaint()));

   
  m_pPaintImage = new QImage(QSize(CCamera::getInstance().getWidth(),
				   CCamera::getInstance().getHeight()),
			     QImage::Format_RGB888);
  QPainter imagePainter(m_pPaintImage);
  imagePainter.initFrom(this);
  imagePainter.setRenderHint(QPainter::Antialiasing, true);
  imagePainter.eraseRect(m_pPaintImage->rect());
  imagePainter.drawEllipse(QPoint(m_pPaintImage->width()/2,
				  m_pPaintImage->height()/2),
			   m_pPaintImage->width()/4,
			   m_pPaintImage->width()/4);
  imagePainter.end();
  m_pRubberBand = 0;
  m_pPaintRect = new QRect();

  m_pRubberBand = new QRubberBand(QRubberBand::Rectangle,this);
  m_bRubberVisible = false;
  m_mode = onlyImage;

  // init pen
  m_penRoi.setColor(QColor(0, 0, 255));
  m_penRoi.setWidth(10);
  m_penPos.setColor(QColor(255,0,0));
  m_penPos.setWidth(10);
  m_penCross.setColor(QColor(0, 255,0));
  //m_penCross.setWidth(10);
}


void
CPaintWidget::refreshPaint()
{
  cout<<"CPaintWidget::refreshPaint()"<<endl;

  QImage image(CCamera::getInstance().getData(),
	       CCamera::getInstance().getWidth(),
	       CCamera::getInstance().getHeight(),
	       QImage::Format_RGB888);
  
  QPainter imagePainter(m_pPaintImage);
  imagePainter.initFrom(this);
  imagePainter.setRenderHint(QPainter::Antialiasing, true);
  imagePainter.eraseRect(m_pPaintImage->rect());

  // image
  imagePainter.drawImage(m_pPaintImage->rect(), image, image.rect());

  // cross
  imagePainter.setPen(m_penCross);
  imagePainter.drawLine(0,
			m_pPaintImage->height()/2,
			m_pPaintImage->width(),
			m_pPaintImage->height()/2);
  imagePainter.drawLine(m_pPaintImage->width()/2,
			0,
			m_pPaintImage->width()/2,
			m_pPaintImage->height());

  switch (m_mode){
  case onlyImage:
    break;
  case withLocateArea:
    break;
  case withOneChip:
    imagePainter.setPen(m_penRoi);
    imagePainter.drawRect(m_firstChipRoi.normalized());
    
    imagePainter.setPen(m_penPos);
    imagePainter.drawRect(m_firstChipPos.normalized());
    break;
  case withTwoChip:
    imagePainter.setPen(m_penRoi);
    imagePainter.drawRect(m_firstChipRoi.normalized());
    imagePainter.drawRect(m_secondChipRoi.normalized());
    
    imagePainter.setPen(m_penPos);
    imagePainter.drawRect(m_firstChipPos.normalized());
    imagePainter.drawRect(m_secondChipPos.normalized());

    imagePainter.setPen(m_penRoi);
    imagePainter.drawLine(CDataSet::getInstance().getFirstChipCenter().x,
			  CDataSet::getInstance().getFirstChipCenter().y,
			  CDataSet::getInstance().getSecondChipCenter().x,
			  CDataSet::getInstance().getSecondChipCenter().y);
    break;
  case withBaseAndChip:
    imagePainter.setPen(m_penRoi);
    imagePainter.drawRect(m_firstChipRoi.normalized());
    
    imagePainter.setPen(m_penPos);
    imagePainter.drawRect(m_firstChipPos.normalized());
    
  case withBase:{
    imagePainter.setPen(m_penRoi);
    imagePainter.drawRect(m_baseRoi.normalized());
    
    imagePainter.setPen(m_penPos);
    imagePainter.drawLine(m_basePosP1, m_basePosP2);
    imagePainter.drawLine(m_basePosP2, m_basePosP3);
    imagePainter.drawLine(m_basePosP3, m_basePosP1);
  }
    break;
  default:
    break;
    }
  
  
  imagePainter.end();
  
  update();

}


void
CPaintWidget::paintEvent(QPaintEvent* event){
  //cout<<"CPaintWidget::paintEvent()"<<endl;
  if (m_pPaintImage != NULL){
    QPainter widgetPainter(this);
    
    widgetPainter.drawImage(*m_pPaintRect,*m_pPaintImage);
  }
}

void
CPaintWidget::resizeEvent(QResizeEvent* event){

  
  if (m_pPaintImage->size().height() ==0 ||
      m_pPaintImage->size().height() ==0){
    
    cout<<"CPaintWidget::resizeEvent() ==0"<<endl;
    return;
  }
  float l_fEventRatio = float(event->size().width() ) /
                        float( event->size().height());
  float l_fPaintRatio = float(m_pPaintImage->size().width()) /
                        float(m_pPaintImage->size().height());

  int adjHeight, adjWidth = 0;
  if ( l_fEventRatio<l_fPaintRatio ){
    adjWidth = event->size().width();
    adjHeight = event->size().width()
                *m_pPaintImage->size().height()
                /m_pPaintImage->size().width();
  }else if( l_fEventRatio>l_fPaintRatio ){
    adjWidth = event->size().height()
              *m_pPaintImage->size().width()
              /m_pPaintImage->size().height(); 
    adjHeight = event->size().height();
  }else{
    adjWidth = event->size().width();
    adjHeight = event->size().height();
    return;
  }
  
  m_pPaintRect->setLeft(event->size().width()/2 - adjWidth/2);
  m_pPaintRect->setRight(event->size().width()/2 + adjWidth/2);
  m_pPaintRect->setTop(event->size().height()/2 - adjHeight/2);
  m_pPaintRect->setBottom(event->size().height()/2 + adjHeight/2);

  // adjust rubber
  if (!m_pPaintRect->contains(origin)){
    origin = m_pPaintRect->topLeft();
  }
  if (!(m_pPaintRect->contains(cur))){
    cur = m_pPaintRect->bottomRight();
  }
}


void
CPaintWidget::mousePressEvent(QMouseEvent* event)
{
  if (! m_pPaintRect->contains(event->pos())){
    return ;
  }

  if (m_bRubberVisible){
    origin = event->pos();
    m_pRubberBand->setGeometry(QRect(origin, cur));
  }
}
void
CPaintWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (m_bRubberVisible){
    cur = event->pos();

      
    if (cur.x() < m_pPaintRect->left()){
      cur.setX( m_pPaintRect->left());
    }else if (cur.x() > m_pPaintRect->right()){
      cur.setX(m_pPaintRect->right());
    }
    if (cur.y() < m_pPaintRect->top()){
      cur.setY(m_pPaintRect->top());
    }else if (cur.y() > m_pPaintRect->bottom()){
	cur.setY(m_pPaintRect->bottom());
    }
    
    m_pRubberBand->setGeometry(QRect(origin, cur).normalized());
  }
}



void
CPaintWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (m_bRubberVisible){
    m_rubberRect=QRect(origin, cur).normalized();
    m_selectedRect.setTopLeft(guiToImg(m_rubberRect.topLeft()));
    m_selectedRect.setBottomRight(guiToImg(m_rubberRect.bottomRight()));
    ostringstream ostm;
    ostm <<"m_rubberRect: "
	 << " top() :"<< m_rubberRect.top()
	 << " bottom():" << m_rubberRect.bottom()
	 << " left(): " << m_rubberRect.left()
	 << " right():" <<m_rubberRect.right()
	 <<endl;
    
    ostm <<"m_selectedRect: "
	 << " top() :"<< m_selectedRect.top()
	 << " bottom():" << m_selectedRect.bottom()
	 << " left(): " << m_selectedRect.left()
	 << " right():" <<m_selectedRect.right()
	 <<endl;
    /*
    ostm  <<"m_pPaintRect:"
	  <<" top() :"  << m_pPaintRect->top()
	  <<" bottom(): "<<m_pPaintRect->bottom()
	  <<" left(): " << m_pPaintRect->left()
	  <<" right():" << m_pPaintRect->right()
	  <<endl;
    */
    m_pList->addItem(QString::fromStdString(ostm.str()));
  }
}


  
QPoint
CPaintWidget::guiToImg(const QPoint& _point){
  QPoint _p = QPoint();
  _p.setX( (_point.x()- size().width()/2) * m_pPaintImage->width()
	   /m_pPaintRect->width() + m_pPaintImage->width()/2);
  _p.setY( (_point.y()- size().height()/2) * m_pPaintImage->height()
	   /m_pPaintRect->height() + m_pPaintImage->height()/2);
  return _p;
}


QPoint
CPaintWidget::imgToGui(const QPoint& _point){
  QPoint _p = QPoint();

  return _p;
}
  
void
CPaintWidget::setRubberVisible(bool visible){
  m_bRubberVisible = visible;
  if (visible == true){
    m_pRubberBand->setGeometry(QRect(origin, cur).normalized());
    m_pRubberBand->show();
    
  }else{
    m_pRubberBand->hide();
  }  
  
}
