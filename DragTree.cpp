// DragTree.cpp : implementation file
//

#include "stdafx.h"
#include "DragTreeItem.h"
#include "DragTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragTree

CDragTree::CDragTree()
{
	m_pDragImages = NULL;
	m_bDrag = FALSE;
	m_hBeginDrag = NULL;
}

CDragTree::~CDragTree()
{
}


BEGIN_MESSAGE_MAP(CDragTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CDragTree)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragTree message handlers

void CDragTree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;		//��ȡ��ʼ�϶��Ľڵ�
	if (hItem==GetRootItem())							//�������϶����ڵ�
	{
		*pResult = 0;
		return;
	}
	m_hBeginDrag = hItem;								//��¼��ʼ�϶�����Ŀ
	m_pDragImages = CreateDragImage(hItem);				//�����϶���ͼ���б�

	CPoint dragPT;										//��¼��ʼ��
	dragPT.x = pNMTreeView->ptDrag.x;
	dragPT.y = pNMTreeView->ptDrag.y;
	if (m_pDragImages != NULL)
	{
		m_pDragImages->BeginDrag(0, CPoint(8, 8));		//��ʼ�϶�ͼ��
		ClientToScreen(&dragPT);						//ת���ͻ����굽��Ļ����
		m_pDragImages->DragEnter(this, dragPT);			//�������ڸ��£����϶��Ĺ�������ʾ�϶���ͼ��
		SetCapture();									//��ʼ��겶׽
		m_bDrag = TRUE;
	}
	*pResult = 0;
}

void CDragTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDrag)										//�����϶�״̬
	{
		HTREEITEM	hItem;
		UINT		nHitFlags;
		CRect		clientRC;
		GetClientRect(&clientRC);						//��ȡ�ͻ�����
		m_pDragImages->DragMove(point);					//�����϶���ͼ��λ��
		//��꾭��ʱ������ʾ
		if( (hItem = HitTest(point, &nHitFlags)) != NULL )
		{
			CImageList::DragShowNolock(FALSE);			//�����϶���ͼ��
			SelectDropTarget(hItem);					//����ѡ�е���Ŀ
			CImageList::DragShowNolock(TRUE);			//��ʾ�϶���ͼ��
		}
	}
	else
		CTreeCtrl::OnMouseMove(nFlags, point);
}

void CDragTree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDrag)										//�����϶�״̬
	{
		m_bDrag = FALSE;
		CImageList::DragLeave(this);					//
		CImageList::EndDrag();							//����ͼ���϶�
		ReleaseCapture();								//�ͷ���겶׽
		delete m_pDragImages;							//�ͷ�ͼ���б�
		m_pDragImages = NULL;
		CRect winRC;
		GetWindowRect(&winRC);							//��ȡ��������
		HTREEITEM hItem;
		if((hItem = HitTest(point, &nFlags)) != NULL)
		{
			//�����϶�����
			//���Ŀ����Ŀ�뿪ʼ�϶�����Ŀ��ͬ����Ŀ����Ŀ���ǿ�ʼ��Ŀ�ĸ��ڵ㣬�����д���
			if (m_hBeginDrag != hItem && hItem != GetParentItem(m_hBeginDrag))
			{
				CopyNodes(hItem, m_hBeginDrag);			//���нڵ�ĸ���
				DeleteItem(m_hBeginDrag);				//ɾ��Դ�ڵ�
			}
			Invalidate();
			SelectDropTarget(NULL);
			m_hBeginDrag = NULL;
		}
	}
}

//���ƽڵ㣬��hSrcItem��������Ŀ���Ƶ���hDesItem�ڵ���
void CDragTree::CopyNodes(HTREEITEM hDesItem, HTREEITEM hSrcItem)
{

	if (hDesItem==NULL || hSrcItem==NULL)				//��֤����
	{
		return;
	}
	TVITEM tvItem;										//������Ŀ��Ϣ
	tvItem.mask = TVIF_TEXT|TVIF_IMAGE;					//���÷��ر��
	tvItem.hItem = hSrcItem;
	char chText[MAX_PATH] = {0};
	tvItem.pszText = chText;
	tvItem.cchTextMax = MAX_PATH;
	GetItem(&tvItem);									//��ȡ��Ŀ��Ϣ
	TVINSERTSTRUCT tvInsert;							//���������������ݽṹ
	tvInsert.hParent = hDesItem;
	tvInsert.item = tvItem;
	HTREEITEM hInsert = InsertItem(&tvInsert);			//������Ŀ
	HTREEITEM hChild = GetChildItem(hSrcItem);			//��ȡ�ӽڵ�
	while (hChild != NULL)								//�����ӽڵ�
	{
		tvItem.mask = TVIF_TEXT|TVIF_IMAGE;
		tvItem.hItem = hChild;
		tvItem.pszText = chText;
		tvItem.cchTextMax = MAX_PATH;
		GetItem(&tvItem);
		tvInsert.hParent = hInsert;
		tvInsert.item = tvItem;
		CopyNodes(hInsert, hChild);						//�ݹ����
		hChild = GetNextSiblingItem(hChild);			//������һ���ֵܽڵ�	
	}
}



