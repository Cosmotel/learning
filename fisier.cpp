
// MFCAppDlg.cpp : implementation file
//
#include "stdafx.h"
#include "MFCApp.h"
#include "MFCAppDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <time.h>
#include <ctime>
#include <iostream>
#include "PerfTimer.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMFCAppDlg::CMFCAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCAppDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CMFCAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CMFCAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CurveVectorPtr CreateBentleyRoad();
CPolyX CreateLeapRoad();
void MyDumpPoly(CPolyX myPoly);
void MyDumpBentley(CurveVectorPtr curveVector);

/**********************************************************/

const double INFINIT		= 1.0e12;

typedef struct tagVerteXBentley{
	DPoint3d	m_pt;
	double		m_delta;
	int			m_eType;
}VerteXBentley;

struct CurveItem
{
	ICurvePrimitivePtr curvePrimitive;
	double length;
};

bvector<CurveItem> CurveData;

typedef CArray<VerteXBentley, VerteXBentley&> VerteXArrayBentley;
typedef CArray<DPoint3d, DPoint3d&> DPoint3dArray;

typedef struct tagPolyXBentley{
	double				m_len;
	BOOL				m_closed;
	VerteXArrayBentley	m_v;
	CurveVectorPtr		m_curve;
}PolyXBentley;

class CPolyXBentley : public tagPolyXBentley
{
public:
	CPolyXBentley    ();
	CPolyXBentley    (const CPolyXBentley& poly);
	CPolyXBentley    (const DPoint3dArray& pts);
	virtual                ~CPolyXBentley    (){};

// Operations
public:
	void					InitVert         (VerteXBentley* v);
	void					DeleteVert		 (int vert);
	void					InsertVert		 (int insVertAt, const DPoint3d *initV, double delta, int type);
	void					InsertVert		 (int vPos, VerteXBentley& vertex);
	void                    Reset            ();
	double			        Length2D         () const;
	double			        Length3D         () const;
	double			        RealLength3D     () const;
	void			        Offset           (double offset);
	void			        Offset           (CurveVectorPtr poly, double offset);
	void			        SetOffAlg		 (DPoint3dR point, double offset, double along) const;
	BOOL			        GetOffAlg	     (DPoint3d coord, double* offset, double* along) const;
	int                     IntWith          (DSegment3dP sgm, CurveVectorPtr coord) const;
	int                     IntWith          (DEllipse3dP sgm, CurveVectorPtr coord) const;
	int				        IntWith          (CurveVectorPtr poly2, CurveVectorPtr coord) const; 
	//
	void                    SetupCurveData   (CurveVectorPtr curveVector);
	//
	void					Copy             (const CPolyXBentley &poly);
	BOOL					Equal			 (const CurveVectorPtr& poly) const;
	void					Reverse			 ();
	int						NumberOfArcs	 () const;

	double					GetMinX			 () const;
	double					GetMinY			 () const;
	double					GetMinZ			 () const;

    double					GetMaxX			 () const;
    double					GetMaxY			 () const;
	double					GetMaxZ			 () const;

	int						GetSize			 (void) const;// Returns the number of vertices in the polygon.
	DPoint3d				GetAt			 (int zPos) const;// Returns the coordinates of a vertex.
	DPoint3d&				operator []		 (int zPos);
	DPoint3d				operator []		 (int zPos) const;

	CurveVectorPtr			Add				 (CurveVectorPtr& poly2); //  + poly2
	CurveVectorPtr          Add				 (DPoint3dR coord) const; //  + coord
	void					AddEqual		 (CurveVectorPtr& poly2); //  += poly2
	void					AddEqual         (const DPoint3dR coord); //  += coord
	void					AddEqual		 (DSegment3dR segment);   //  += segment
	void					AddEqual		 (DEllipse3dR arcB);	  //  += arc

	BOOL					IsClosed		 ();

	BOOL					MyDump			 (CurveVectorPtr dump);

};

CPolyXBentley::CPolyXBentley()
{
	Reset();
}

void CPolyXBentley::Reset()
{
	m_len = 0.0e0;
	m_closed = FALSE;
	m_v.RemoveAll();
	if(m_curve.IsValid())
		m_curve->clear();
}

void CPolyXBentley::InitVert(VerteXBentley* v)
{
	ASSERT(v);

	v->m_delta = 0.0e0;
	v->m_eType = 0;
	v->m_pt.x = 0.0e0;
	v->m_pt.y = 0.0e0;
	v->m_pt.z = 0.0e0;
}

void CPolyXBentley::InsertVert(int vPos, VerteXBentley& vertex)
{
	m_v.InsertAt(vPos, vertex);
}

void CPolyXBentley::DeleteVert(int vert)
{
	int lim = m_v.GetSize();
	if (lim > 1 && vert >= 0 && vert < lim) {
		m_v.RemoveAt(vert);
	}
	else {
#if defined(_DEBUG)
		MessageBeep(MB_OK);
#endif
	}
}

void CPolyXBentley::InsertVert(int insVertAt, const DPoint3d *initV, double delta, int type)
{
	VerteXBentley v;
	InitVert(&v);
	v.m_delta = 0.0e0;
	v.m_eType = type;
	v.m_pt = *initV;
    if(delta != 0.0e0) {
		v.m_delta = delta;
		v.m_eType = 1;
	}
	m_v.InsertAt(insVertAt, v);
}

CPolyXBentley::CPolyXBentley(const CPolyXBentley& poly)
{
	Copy(poly);
}

void CPolyXBentley::Copy(const CPolyXBentley &poly)
{
	int nodes = poly.m_v.GetSize();
	m_v.SetSize(nodes);
	for (int i = 0;  i < nodes;  i++ ) {
		m_v[i] = poly.m_v[i];
	}
	m_closed = poly.m_closed;
	m_len = poly.m_len;

	m_curve = poly.m_curve->Clone();
}

double CPolyXBentley::Length2D() const
{
	return this->m_curve->FastLength();
}

double CPolyXBentley::Length3D() const
{
	return this->m_curve->FastLength();
}

double CPolyXBentley::RealLength3D() const
{
	int nrCurves = this->m_curve->CountPrimitivesBelow();
	double realLength = 0.0e0;
	DPoint3d pt1, pt2;
	ICurvePrimitivePtr curvePrimitive;
	double tempLength = 0.0e0;

	for(int i = 0 ; i < nrCurves; i++)
	{
		curvePrimitive = this->m_curve->GetCyclic (i);
		curvePrimitive->GetStartEnd(pt1, pt2);
		
		ICurvePrimitivePtr segment = ICurvePrimitive::
		CreateLine (DSegment3d::From(pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z));
		segment->FastLength(tempLength);

		realLength+=tempLength;
	}
	return realLength;
}

void CPolyXBentley::Offset(double offset)
{
	m_curve = m_curve->CloneOffsetCurvesXY(CurveOffsetOptions::CurveOffsetOptions(offset));
}

void CPolyXBentley::Offset(CurveVectorPtr poly, double offset)
{
	poly = poly->CloneOffsetCurvesXY(CurveOffsetOptions::CurveOffsetOptions(offset));
}

void CPolyXBentley::SetOffAlg(DPoint3dR point, double offset, double along) const
{
	double tempLength = 0.0e0;
	point.Init(0, 0, 0);

	int idx = 0;
	for (size_t i = 0; i < CurveData.size(); i++)
	{
		if (along > tempLength || along < tempLength)
		{
			idx = i;
			tempLength += CurveData[i].length;
		}
		else
			break;
	}

	ICurvePrimitivePtr curvePrimitive = this->m_curve->GetCyclic(idx);
	double length = CurveData[idx].length;

	DRay3d ray1;
	double fraction = (along + length - tempLength) / length;
	
	if(fraction < 0.0)
		curvePrimitive->FractionToPoint (-fraction, ray1);
	else
		curvePrimitive->FractionToPoint (fraction, ray1);

	ray1.direction.normalize();

	DVec3d vOZ = DVec3d::From (0, 0, 1);
	DVec3d v2;
	v2.crossProduct (&ray1.direction, &vOZ);

	v2.normalize();
	point = ray1.origin;
	if (offset != 0)
	{
		v2.scale(offset);
		point.add(&v2);
	}
}

BOOL CPolyXBentley::GetOffAlg(DPoint3d coord, double* offset, double* along) const
{
	bool result = true;
	*offset = 0.0e0;
	*along = 0.0e0;
	double length = 0.0;

	length = this->m_curve->FastLength();

	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();
	CurveLocationDetail location;

	bool extend0 = false;
	bool extend1 = false;

	
	this->m_curve->ClosestPointBounded ( coord, location, extend0, extend1 );

	size_t idxLocation = this->m_curve->FindIndexOfPrimitive(location.curve);
	size_t nCurves = this->m_curve->CountPrimitivesBelow();
	
	if(idxLocation == nCurves-1 || location.fraction == 0)
	{
		extend0 = extend1 = true;
		this->m_curve->ClosestPointBoundedXY ( coord, &worldToLocal, location, extend0, extend1 );
		idxLocation = this->m_curve->FindIndexOfPrimitive(location.curve);
	}

	length = CurveData[idxLocation].length;

	for (size_t i = 0; i < idxLocation; i++)
	{
		*along += CurveData[i].length;
	}
	*along += length * location.fraction;

	DRay3d ray;
	location.curve->FractionToPoint (location.fraction, ray);

	*offset = location.point.Distance(coord);

	DVec3d v;
	v = DVec3d::FromStartEnd (location.point, coord);

	DVec3d vOZ;
	vOZ.crossProduct (&v, &ray.direction);

	// 	*offset = v.magnitude();
	if (vOZ.z < 0)
		*offset *= -1;
	
	return result;
}

int  CPolyXBentley::IntWith(DEllipse3dP arc, CurveVectorPtr coord) const
{
	int intersect = 0;
	ICurvePrimitivePtr arcPrimitive = ICurvePrimitive::CreateArc (*arc);

	CurveVectorPtr poly2 = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	poly2->Add(arcPrimitive);

	coord = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open); 
	CurveVectorPtr curveB = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();

	CurveCurve::IntersectionsXY(*coord, *curveB, *this->m_curve, *poly2, &worldToLocal);

	DPoint3d startPointA, startPointB;
	coord->GetStartPoint(startPointA);
	curveB->GetStartPoint(startPointB);

	if(startPointA.IsEqual(startPointB, 1e-3))
		intersect = 1;

	return intersect;

}

int CPolyXBentley::IntWith(DSegment3dP sgm, CurveVectorPtr coord) const
{
	int intersect = 0;
	ICurvePrimitivePtr line1 = ICurvePrimitive::CreateLine (*sgm);

	CurveVectorPtr poly2 = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	poly2->Add(line1);

	coord = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open); 
	CurveVectorPtr curveB = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();

	CurveCurve::IntersectionsXY(*coord, *curveB, *this->m_curve, *poly2, &worldToLocal);

	DPoint3d startPointA, startPointB;
	coord->GetStartPoint(startPointA);
	curveB->GetStartPoint(startPointB);

	if(startPointA.IsEqual(startPointB, 1e-3))
		intersect = 1;

	return intersect;
}

int CPolyXBentley::IntWith(CurveVectorPtr poly2, CurveVectorPtr coord) const
{
	int intersect = 0;
	coord = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);  
	CurveVectorPtr curveB = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();

	CurveCurve::IntersectionsXY(*coord, *curveB, *m_curve, *poly2, &worldToLocal);
	
	DPoint3d startPointA, startPointB;
	coord->GetStartPoint(startPointA);
	curveB->GetStartPoint(startPointB);

	if(startPointA.IsEqual(startPointB, 1e-3))
		intersect = 1;

	return intersect;
}

int CPolyXBentley::NumberOfArcs() const
{
	int numberArcs = 0;

	numberArcs = m_curve->CountPrimitivesOfType(ICurvePrimitive::CURVE_PRIMITIVE_TYPE_Arc);

	return numberArcs;
}

int CPolyXBentley::GetSize() const
{
	return m_v.GetSize();
}

DPoint3d CPolyXBentley::GetAt(int zPos) const
{
	return m_v.GetAt(zPos).m_pt;
}

double CPolyXBentley::GetMinX() const
{
    double MinX = INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).x < MinX)
            MinX = GetAt(i).x;
    }
    return MinX;
}

double CPolyXBentley::GetMinY() const
{
    double MinY = INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).y < MinY)
            MinY = GetAt(i).y;
    }
    return MinY;
}

double CPolyXBentley::GetMinZ() const
{
    double MinZ = INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).z < MinZ)
            MinZ = GetAt(i).z;
    }
    return MinZ;
}

double CPolyXBentley::GetMaxX() const
{
    double MaxX = -INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).x > MaxX)
            MaxX = GetAt(i).x;
    }
    return MaxX;
}

double CPolyXBentley::GetMaxY() const
{
    double MaxY = -INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).y > MaxY)
            MaxY = GetAt(i).y;
    }
    return MaxY;
}

double CPolyXBentley::GetMaxZ() const
{
    double MaxZ = -INFINIT;
    for(int i = 0; i < GetSize(); i++)
    {
        if(GetAt(i).z > MaxZ)
            MaxZ = GetAt(i).z;
    }
    return MaxZ;
}

DPoint3d& CPolyXBentley::operator [] (int zPos)
{
	return m_v[zPos].m_pt;
}

DPoint3d CPolyXBentley::operator [] (int zPos) const
{
	return m_v[zPos].m_pt;
}

BOOL CPolyXBentley::IsClosed()
{
	BOOL pathClosed = false;

	pathClosed = m_curve->IsClosedPath();

	return pathClosed;
}

CurveVectorPtr CPolyXBentley::Add(CurveVectorPtr& poly2)
{
	CurveVectorPtr result;

	if(m_curve.IsNull())
		m_curve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);
	
	result = m_curve->Clone();

	if(poly2.IsValid())
		result->AddPrimitives(*poly2);

	return result;
}

CurveVectorPtr CPolyXBentley::Add(DPoint3dR pt2) const
{
	int nrCurves;
	double realLength = 0.0e0;
	DPoint3d pt1;
	ICurvePrimitivePtr segment;
	ICurvePrimitivePtr curvePrimitive;
	CurveVectorPtr temp;
	
	if(m_curve.IsValid())
	{	
		nrCurves = m_curve->CountPrimitivesBelow();
		curvePrimitive = m_curve->GetCyclic (nrCurves-1);
		curvePrimitive->GetStartEnd(pt1, pt2);
		segment = ICurvePrimitive::CreateLine (DSegment3d::From(pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z));
		temp = m_curve->Clone();
		temp->Add(segment);
	}
	return temp;
}

void CPolyXBentley::AddEqual(CurveVectorPtr& poly2)
{
	if(m_curve.IsNull())
		m_curve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);
	
	if(poly2.IsValid())
		m_curve->AddPrimitives(*poly2->Clone());
}

void CPolyXBentley::AddEqual(const DPoint3dR pt2)
{
	int nrCurves;
	double realLength = 0.0e0;
	DPoint3d pt1;
	ICurvePrimitivePtr segment;
	ICurvePrimitivePtr curvePrimitive;
	
	if(m_curve.IsValid())
	{	
		nrCurves = m_curve->CountPrimitivesBelow();
		curvePrimitive = m_curve->GetCyclic (nrCurves-1);
		curvePrimitive->GetStartEnd(pt1, pt2);
		segment = ICurvePrimitive::CreateLine (DSegment3d::From(pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z));
		CurveVectorPtr tempCurve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);
		tempCurve->Add(segment);
		m_curve->AddPrimitives(*tempCurve);
	}
}

void CPolyXBentley::AddEqual(DSegment3dR segm)
{
	if(m_curve.IsNull())
		m_curve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);

	ICurvePrimitivePtr segment = ICurvePrimitive::CreateLine (segm);
	CurveVectorPtr tempCurve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);
	tempCurve->Add(segment);

	m_curve->AddPrimitives(*tempCurve);
}

void CPolyXBentley::AddEqual(DEllipse3dR arcB)
{
	if(m_curve.IsNull())
		m_curve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);

	ICurvePrimitivePtr arc = ICurvePrimitive::CreateArc(arcB);
	CurveVectorPtr tempCurve = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Open);
	tempCurve->Add(arc);

	m_curve->AddPrimitives(*tempCurve);
}

void CPolyXBentley::SetupCurveData(CurveVectorPtr curveVector)
{
	CurveData.clear();
	int nCurves = curveVector->CountPrimitivesBelow();
	if (0 == nCurves)
		return;

	for (int i = 0; i < nCurves; i++)
	{
		CurveItem item;
		item.curvePrimitive = curveVector->GetCyclic (i);
		item.curvePrimitive->FastLength (item.length);
		CurveData.push_back(item);
	}
}

BOOL CPolyXBentley::Equal(const CurveVectorPtr& poly) const
{
	BOOL isSame = false;

	isSame = m_curve->IsSameStructure(*poly);

	return isSame;
}

void CPolyXBentley::Reverse()
{
	m_curve = m_curve->CloneReversed();
}

BOOL CPolyXBentley::MyDump(CurveVectorPtr dump)
{
	BOOL rez = false;
	MSBsplineCurvePtr bsplineCurveptr = dump->GetBsplineCurve();

	if(bsplineCurveptr.IsNull())
		abort();

	double lung = dump->FastLength();

	if(lung > 0.0e0)
		rez = true;

	ATLTRACE ("Fraction to point for Bentley .DLL\n");
	for (int i = 0; i <= lung ; i++)
	{
		DPoint3d xyz;
		bsplineCurveptr->FractionToPoint (xyz, (1.0f / 20) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");

	return rez;
}
/**********************************************************/

void SetupCurveData(CurveVectorPtr curveVector)
{
	CurveData.clear();
	int nCurves = curveVector->CountPrimitivesBelow();
	if (0 == nCurves)
		return;

	for (int i = 0; i < nCurves; i++)
	{
		CurveItem item;
		item.curvePrimitive = curveVector->GetCyclic (i);
		item.curvePrimitive->FastLength (item.length);
		CurveData.push_back(item);
	}
}

/// <summary>
/// Bentleys the get offset and along.
/// </summary>
/// <param name="curveVector">The curve vector.</param>
/// <param name="coord">The coord.</param>
/// <param name="offset">The offset.</param>
/// <param name="along">The along.</param>
/// <returns></returns>
BOOL BentleyGetOffAlg(CurveVectorPtr curveVector, DPoint3d coord, double* offset, double* along)//ii dau punctul sa aflu offsetul si alungirea
{
	bool result = true;
	*offset = 0.0e0;
	*along = 0.0e0;
	double length = 0.0;

	length = curveVector->FastLength();

	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();
	CurveLocationDetail location;

	bool extend0 = false;
	bool extend1 = false;

	
	curveVector->ClosestPointBounded ( coord, location, extend0, extend1 );

	size_t idxLocation = curveVector->FindIndexOfPrimitive(location.curve);
	size_t nCurves = curveVector->CountPrimitivesBelow();
	
	if(idxLocation == nCurves-1 || location.fraction == 0)
	{
		extend0 = extend1 = true;
		curveVector->ClosestPointBoundedXY ( coord, &worldToLocal, location, extend0, extend1 );
		idxLocation = curveVector->FindIndexOfPrimitive(location.curve);
	}

	length = CurveData[idxLocation].length;

	for (size_t i = 0; i < idxLocation; i++)
	{
		*along += CurveData[i].length;
	}
	*along += length * location.fraction;

	DRay3d ray;
	location.curve->FractionToPoint (location.fraction, ray);

	*offset = location.point.Distance(coord);

	DVec3d v;
	v = DVec3d::FromStartEnd (location.point, coord);

	DVec3d vOZ;
	vOZ.crossProduct (&v, &ray.direction);

	// 	*offset = v.magnitude();
	if (vOZ.z < 0)
		*offset *= -1;
	
	return result;
}

/// <summary>
/// Bentleys the set off alg old.
/// </summary>
/// <param name="curveVector">The curve vector.</param>
/// <param name="point">The point.</param>
/// <param name="offset">The offset.</param>
/// <param name="along">The along.</param>
void BentleySetOffAlgOld(CurveVectorPtr curveVector, DPoint3dR point, double offset, double along)//ii dau offsetul si alungirea sa aflu punctul
{
#if 0
	double tempLength = 0;

	int nCurves = curveVector->CountPrimitivesBelow();
	if (0 == nCurves)
		return;

	ICurvePrimitivePtr curvePrimitive = curveVector->GetCyclic(0);

	//size_t myvar = curveVector->CyclicIndex(12);//5

	for (size_t i = 0; i < nCurves; i++)
	{
		if (along > tempLength)
		{
			curvePrimitive = curveVector->GetCyclic (i);
			double length = 0;
			curvePrimitive->FastLength (length);
			tempLength += length;
		}
		else
			break;
	}

	if (NULL == curvePrimitive.get())
		return;

	double length;
	curvePrimitive->FastLength(length);

	CurveLocationDetail location;
	curvePrimitive->PointAtSignedDistanceFromFraction (0, along + length-tempLength, true, location);
	point = location.point;

	DRay3d ray;
	location.curve->FractionToPoint (location.fraction + 0.01f, ray);

	DVec3d v2 = DVec3d::FromCCWPerpendicularXY (DVec3d::FromStartEnd (ray.origin, location.point));
	v2.normalize();
	if (offset != 0)
	{
		v2.scale(offset);
		point.x = location.point.x;
		point.y = v2.y;
	}
	else
		point = location.point;
#endif

#if 1
	double tempLength = 0;
	point.Init(0, 0, 0);

	int idx = 0;
	for (size_t i = 0; i < CurveData.size(); i++)
	{
		if (along > tempLength || along < tempLength)
		{
			idx = i;
			tempLength += CurveData[i].length;
		}
		else
			break;
	}

	ICurvePrimitivePtr curvePrimitive = curveVector->GetCyclic(idx);
	double length = CurveData[idx].length;

	DRay3d ray1;
	double fraction = (along + length - tempLength) / length;
	
	if(fraction < 0.0)
		curvePrimitive->FractionToPoint (-fraction, ray1);
	else
		curvePrimitive->FractionToPoint (fraction, ray1);

	ray1.direction.normalize();

	DVec3d vOZ = DVec3d::From (0, 0, 1);
	DVec3d v2;
	v2.crossProduct (&ray1.direction, &vOZ);

	v2.normalize();
	point = ray1.origin;
	if (offset != 0)
	{
		v2.scale(offset);
		point.add(&v2);
	}

#endif

}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    daniel.ilisoi                   11/2014
+---------------+---------------+---------------+---------------+---------------+------*/
void BentleySetOffAlg(CurveVectorPtr curveVector, DPoint3dR point, double offset, double along)
{
	double tempLength = 0;
	point.Init(0, 0, 0);
	if (false)
	{
		int nCurves = curveVector->CountPrimitivesBelow();
		if (0 == nCurves)
			return;

		ICurvePrimitivePtr curvePrimitive = curveVector->GetCyclic (0);

		for (int i = 0; i < nCurves; i++)
		{
			if (along > tempLength)
			{
				curvePrimitive = curveVector->GetCyclic (i);
				double length = 0;
				curvePrimitive->FastLength (length);
				tempLength += length;
			}
			else
				break;
		}

		if (NULL == curvePrimitive.get())
			return;

		double length;
		curvePrimitive->FastLength (length);
		ATLTRACE("");
	}

	int idx = 0;
	for (size_t i = 0; i < CurveData.size(); i++)
	{
		if (along > tempLength)
		{
			idx = i;
			tempLength += CurveData[i].length;
		}
		else
			break;
	}

	ICurvePrimitivePtr curvePrimitive = curveVector->GetCyclic(idx);
	double length = CurveData[idx].length;

	DRay3d ray1;
	curvePrimitive->FractionToPoint ((along + length - tempLength) / length, ray1);

	point = ray1.origin;
	ray1.direction.normalize();

	// DVec3d v2 = DVec3d::FromCCWPerpendicularXY (ray1.direction);
	//v2.rotateXY(msGeomConst_pi);

	DVec3d vOZ = DVec3d::From (0, 0, 1);
	DVec3d v2;
	v2.crossProduct (&ray1.direction, &vOZ);

	v2.normalize();
	point = ray1.origin;
	if (offset != 0)
	{
		v2.scale(offset);
		point.add(&v2);
	}
}

//models
CPolyX CreateHalfSquareL()
{
	CPolyX myPoly;

	CCoord pnt1(0, 0);
	CCoord pnt2(20, 0);

	CCoord pnt3(20, 20);
	CCoord pnt4(40, 20);

	CCoord pnt5(40, 0);
	CCoord pnt6(60, 0);

	CCoord pnt7(60, 20);
	CCoord pnt8(80, 20);

	CCoord pnt9(80, 0);
	CCoord pnt10(100, 0);

	CCourse line1(pnt1, pnt2);
	CCourse line2(pnt2, pnt3);
	CCourse line3(pnt3, pnt4);
	CCourse line4(pnt5, pnt5);
	CCourse line5(pnt5, pnt6);
	CCourse line6(pnt6, pnt7);
	CCourse line7(pnt7, pnt8);
	CCourse line8(pnt8, pnt9);
	CCourse line9(pnt9, pnt10);

	myPoly+= line1;
	myPoly+= line2;
	myPoly+= line3;
	myPoly+= line4;
	myPoly+= line5;
	myPoly+= line6;
	myPoly+= line7;
	//myPoly+= line8;
	//myPoly+= line9;

	return myPoly;
}

CurveVectorPtr CreateHalfSquareB()
{
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, 0, 0, 20, 0, 0));

	ICurvePrimitivePtr line2 = ICurvePrimitive::
		CreateLine (DSegment3d::From(20, 0, 0, 20, 20, 0));

	ICurvePrimitivePtr line3 = ICurvePrimitive::
		CreateLine (DSegment3d::From(20, 20, 0, 40, 20, 0));

	ICurvePrimitivePtr line4 = ICurvePrimitive::
		CreateLine (DSegment3d::From(40, 20, 0, 40, 0, 0));

	ICurvePrimitivePtr line5 = ICurvePrimitive::
		CreateLine (DSegment3d::From(40, 0, 0, 60, 0, 0));

	ICurvePrimitivePtr line6 = ICurvePrimitive::
		CreateLine (DSegment3d::From(60, 0, 0, 60, 20, 0));

	ICurvePrimitivePtr line7 = ICurvePrimitive::
		CreateLine (DSegment3d::From(60, 20, 0, 80, 20, 0));

	ICurvePrimitivePtr line8 = ICurvePrimitive::
		CreateLine (DSegment3d::From(80, 20, 0, 80, 0, 0));

	ICurvePrimitivePtr line9 = ICurvePrimitive::
		CreateLine (DSegment3d::From(80, 0, 0, 100, 0, 0));


	CurveVectorPtr myCurveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open,
										  line1 
		);

	myCurveVector->Add(line2);
	myCurveVector->Add(line3);
	myCurveVector->Add(line4);
	myCurveVector->Add(line5);
	myCurveVector->Add(line6);
	myCurveVector->Add(line7);
	//myCurveVector->Add(line8);
	//myCurveVector->Add(line9);

	return myCurveVector;
}

CPolyX CreateTriangleModelL()
{
	CPolyX myPoly;

	CCoord pnt1(0, 0);
	CCoord pnt2(10, 10);

	CCoord pnt3(20, 0);
	CCoord pnt4(20, 10);

	CCourse line1(pnt1, pnt2);
	CCourse line2(pnt2, pnt3);
	CCourse line3(pnt3, pnt4);

	myPoly+= line1;
	myPoly+= line2;
	//myPoly+= line3;

	return myPoly;
}

CurveVectorPtr CreateTriangleModelB()
{
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, 0, 0, 10, 10, 0));

	ICurvePrimitivePtr line2 = ICurvePrimitive::
		CreateLine (DSegment3d::From(10, 10, 0, 20, 0, 0));

	ICurvePrimitivePtr line3 = ICurvePrimitive::
		CreateLine (DSegment3d::From(20, 0, 0, 20, 10, 0));

	CurveVectorPtr myCurveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open);
	
	myCurveVector->Add(line1);
	myCurveVector->Add(line2);
	//myCurveVector->Add(line3);

	return myCurveVector;
}

CPolyX CreateTwoSegmentsLeap()
{
	CPolyX myPoly;

	CCoord pnt1(0, -200);
	
	CCoord pnt2(500, -200);

	CCoord pnt3(500, -300);

	CCoord pnt4(800, -300);

	CCourse line1(pnt1, pnt2);//first segment
	CCourse line2(pnt2, pnt3);//second segment
	CCourse line3(pnt3, pnt4);//3rd segment

	myPoly+= line1;
	myPoly+= line2;
	myPoly+= line3;

	return myPoly;

}

CurveVectorPtr CreateTwoSegmentsBentley()
{
	//start segment1
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, -200, 0, 500, -200, 0));
	//end segment1

	//start segment2
	ICurvePrimitivePtr line2 = ICurvePrimitive::
		CreateLine (DSegment3d::From(500, -200, 0, 500, -300, 0));
	//end segment2

	//start segment3
	ICurvePrimitivePtr line3 = ICurvePrimitive::
		CreateLine (DSegment3d::From(500, -300, 0, 800, -300, 0));
	//end segment3
	
	CurveVectorPtr myCurveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open);
	myCurveVector->Add(line1);
	myCurveVector->Add(line2);
	myCurveVector->Add(line3);

	return myCurveVector;
}

CurveVectorPtr TestCreateSegmentsBentley()
{
	//start segment1
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, 0, 0, 500, -10, 0));
	//end segment1

	//start segment2
	ICurvePrimitivePtr line3 = ICurvePrimitive::
		CreateLine (DSegment3d::From(-100, 0, 0, 500, -20, 0));
	//end segment2

	//start segment3
	ICurvePrimitivePtr line2 = ICurvePrimitive::
		CreateLine (DSegment3d::From(-200, 0, 0, 500, -30, 0));
	//end segment3
	
	CurveVectorPtr myCurveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open //not expected to close
		);

	myCurveVector->Add(line1);
	myCurveVector->Add(line2);
	myCurveVector->Add(line3);

	MSBsplineCurvePtr bsplineCurveptr = myCurveVector->GetBsplineCurve();

	double lung = bsplineCurveptr->Length();

	ATLTRACE ("Fraction to point for Bentley .DLL\n");
	for (int i = 0; i <= lung ; i++)
	{
		DPoint3d xyz;
		bsplineCurveptr->FractionToPoint (xyz, (1.0f / 20) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");


	return myCurveVector;
}

CPolyX CreateLeapRoad()
{
	CPolyX myPoly;

	CCoord pnt1(0, 0);
	CCoord pnt2(200, 0);

	CCoord pnt3(558.678, -151.6466);
	CCoord pnt4(698.0194, -295.1179);

	CCoord pnt5(1339.2372, -493.4698);
	CCoord pnt6(1358.8385, -489.4964);

	CCoord pnt7(1399.7342, 87.1903);
	CCoord pnt8(740.1785, 321.6820);


	CCourse line1(pnt1, pnt2);//first segment
	CArc arc1(pnt2, CCoord(427.1767, -54.5892), pnt3);//first arc
	CCourse line2(pnt3, pnt4);//2nd segment
	CArc arc2(pnt4, CCoord(1022.0383, -484.3793), pnt5);//2nd arc
	CCourse line3(pnt5, pnt6);//3rd segment
	CArc arc3(pnt6, CCoord(1597.1438, -230.8599), pnt7);//3rd arc
	CCourse line4(pnt7, pnt8);//4th segment

	//add segments and arc to myPoly
	myPoly+= line1;
	myPoly+= arc1;
	myPoly+= line2;
	myPoly+= arc2;
	myPoly+= line3;
	myPoly+= arc3;
	myPoly+= line4;
	//end add

	return myPoly;
}

CurveVectorPtr CreateBentleyRoad()
{
	//start segment1
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, 0, 0, 200, 0, 0));
	//end segment1

	DEllipse3d ellipse;

	//start arc1
	ellipse.InitFromPoints (DPoint3d::From(200,-500,0), DPoint3d::From(700,-500,0), DPoint3d::From(200, 0,0), 1.570796326794897, -0.7999995879196328);
	ICurvePrimitivePtr arc = ICurvePrimitive::CreateArc (ellipse);
	//end arc1

	//start segment2
	ICurvePrimitivePtr line2 = ICurvePrimitive::
		CreateLine (DSegment3d::From(558.678, -151.6466, 0, 698.0194, -295.1179, 0));
	//end segment2

	//start arc2
	ellipse.InitFromPoints (DPoint3d::From(1200.1686, 192.5768 ,0), DPoint3d::From(1900.1686, 192.5768 ,0), DPoint3d::From(1200.1686, 892.5768, 0), 3.912389392465057, 1.000000357564167);
	ICurvePrimitivePtr arc2 = ICurvePrimitive::CreateArc (ellipse);
	//end arc2

	//start segment3
	ICurvePrimitivePtr line3 = ICurvePrimitive::
		CreateLine (DSegment3d::From(1339.2372, -493.4698, 0, 1358.8385, -489.4964, 0));
	//end segment3

	//start arc3
	ellipse.InitFromPoints (DPoint3d::From(1299.2377,-195.4764,0), DPoint3d::From(1599.2377,-195.4764,0), DPoint3d::From(1299.2377,-195.4764+300,0), -1.370795557150362, 2.599999533403433);
	ICurvePrimitivePtr arc3 = ICurvePrimitive::CreateArc (ellipse);
	//end arc3

	//start segment4
	ICurvePrimitivePtr line4 = ICurvePrimitive::
		CreateLine (DSegment3d::From(1399.7342, 87.1903, 0, 740.1785, 321.6820, 0));
	//end segment4


	CurveVectorPtr myCurveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open);

	//begin add segments and arc to myCurveVector
	myCurveVector->Add(line1);
	myCurveVector->Add(arc);
	myCurveVector->Add(line2);
	myCurveVector->Add(arc2);
	myCurveVector->Add(line3);
	myCurveVector->Add(arc3);
	myCurveVector->Add(line4);
	//end add

	return myCurveVector;
}

void CreateMySpiral()
{
/*
#define msGeomConst_piOver4     7.85398163397448280000e-001
#define msGeomConst_piOver2     1.57079632679489660000e+000
#define msGeomConst_pi          3.14159265358979310000e+000
#define msGeomConst_2pi         6.28318530717958620000e+000
#define msGeomConst_degreesPerRadian    (45.0 / msGeomConst_piOver4)
#define msGeomConst_radiansPerDegree    (msGeomConst_piOver4 / 45.0)
*/
	ICurvePrimitivePtr line1 = ICurvePrimitive::
		CreateLine (DSegment3d::From(0, 0, 0, 25, 0, 0));

	//DSpiral2dBaseCP _pmySpiral = DSpiral2dBase::Create(DSpiral2dBase::TransitionType_Clothoid);
	//_pmySpiral->CreateBearingCurvatureLengthCurvature(DSpiral2dBase::TransitionType_Bloss, 10, 30, 60, 40);

	DSpiral2dBloss mySpiralBloss;
	//mySpiralBloss.CreateBearingCurvatureBearingCurvature(DSpiral2dBase::TransitionType_Clothoid, 20, 20, 40, 40);
	//mySpiralBloss.CreateBearingCurvatureLengthCurvature(DSpiral2dBase::TransitionType_Bloss, 0.08, 10, 60, 70);
	mySpiralBloss.Create (DSpiral2dBase::TransitionType_Clothoid);
	
	ICurvePrimitivePtr myCurvePrimitive1 = ICurvePrimitive::
		CreateSpiral (mySpiralBloss, Transform::From(1, 5, 1), 0.1, 0.9);

	ICurvePrimitivePtr myCurvePrimitive2 = ICurvePrimitive::
		CreateSpiralBearingRadiusBearingRadius (DSpiral2dBase::TransitionType_Clothoid, 0.08, 0, 30, 0, Transform::From(5, 1, 1), 0.1, 0.9);

	ICurvePrimitivePtr myCurvePrimitive3 = ICurvePrimitive::
		CreateSpiralBearingCurvatureBearingCurvature (DSpiral2dBase::TransitionType_Clothoid, 0.08, 0, 10, 0, Transform::From(0, 0, 0), 5.5, 10.5);

	CurveVectorPtr curveVector = CurveVector::
		Create (CurveVector::BOUNDARY_TYPE_Open);

	//curveVector->Add(line1);
	curveVector->Add(myCurvePrimitive1);
	//curveVector->Add(myCurvePrimitive2);
	//curveVector->Add(myCurvePrimitive3);
	
	bvector <DPoint3d> points;
	IFacetOptionsPtr options = IFacetOptions::Create();

	curveVector->AddStrokePoints(points, *options);
	
	MSBsplineCurvePtr bsplineCurveptr = curveVector->GetBsplineCurve();
	//double lung = bsplineCurveptr->Length ();

	ATLTRACE ("Fraction to point for spiral.\n");
	for (int i = 0; i <= 100; i++)
	{
		DPoint3d xyz;
		bsplineCurveptr->FractionToPoint ( xyz, (1.0f / 20) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");

}
//end models

//begin dump
void MyDumpPoly(CPolyX myPoly)
{
	myPoly = CreateLeapRoad();

	double lungime = myPoly.Length2D();

	ATLTRACE ("Fraction to point for Leap library\n");
	for (int i = 0; i <= lungime; i+=50)
	{
		CCoord xyz;
		myPoly.SetOffAlg(&xyz, 0, i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");
}

void MyDumpBentley(CurveVectorPtr curveVector)
{
	curveVector = CreateBentleyRoad();
	MSBsplineCurvePtr bsplineCurveptr = curveVector->GetBsplineCurve();

	double lung = curveVector->FastLength();

	ATLTRACE ("Fraction to point for Bentley .DLL\n");
	for (int i = 0; i <= lung ; i++)
	{
		DPoint3d xyz;
		bsplineCurveptr->FractionToPoint (xyz, (1.0f / 20) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");
}
//end dump

/// <summary>
/// Tests the LEAP offset method.
/// </summary>
void TestOffsetLEAP()
{
	CPolyX myPoly = CreateLeapRoad();
	CPolyX myNewPoly;
	myPoly.Offset(&myNewPoly, 100);

	double lungime1 = myPoly.Length3D();
	double lungime2 = myNewPoly.Length3D();

	ATLTRACE ("Points on curve for Leap.\n");
	for (int i = 0; i <= lungime2; i+=50)
	{
		CCoord xyz;
		//myPoly.SetOffAlg(&xyz, 0, i);
		//ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);

		myNewPoly.SetOffAlg(&xyz, 0, i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");
}

/// <summary>
/// Tests the Bentley offset method.
/// </summary>
void TestOffsetBentley()
{
	CurveVectorPtr myCurveVector = CreateBentleyRoad();

	CurveVectorPtr myOffsetCurveVector;
	myOffsetCurveVector = myCurveVector->CloneOffsetCurvesXY(CurveOffsetOptions::CurveOffsetOptions(100));

	MSBsplineCurvePtr bsplineCurveptr1 = myCurveVector->GetBsplineCurve();
	MSBsplineCurvePtr bsplineCurveptr2 = myOffsetCurveVector->GetBsplineCurve();
	
	double lung1 = myCurveVector->FastLength();
	double lung2 = myOffsetCurveVector->FastLength();

	ATLTRACE ("Fraction to point for Bentley.\n");
	for (int i = 0; i <= lung2 ; i++)
	{
		DPoint3d xyz;
		//bsplineCurveptr1->FractionToPoint (xyz, (1.0f / 50) * i);
		//ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);

		bsplineCurveptr2->FractionToPoint (xyz, (1.0f / 50.0f) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");
}

/// <summary>
/// Tests compute point of intersections of two poly in BENTLEY
/// </summary>
void ClosestApproach()
{
	CurveLocationDetail location1, location2;

	CurveVectorPtr myPoly1 = CreateBentleyRoad();

	CurveVectorPtr myPoly2 = CreateTwoSegmentsBentley();

	CurveCurve::ClosestApproach(location1, location2, *myPoly1, *myPoly2);

	DPoint3d myPt1, myPt2;
	myPt1 = location1.point;
	myPt2 = location2.point;

	ASSERT (SameAs (myPt1.x, myPt2.x));
	ASSERT (SameAs (myPt1.y, myPt2.y));

	ATLTRACE( "(BENTLEY) Se intersecteaza la x = %f, y = %f", myPt1.x, myPt1.y);

}

/// <summary>
/// Tests compute point of intersections of two poly in LEAP
/// </summary>
void TestLeapIntWith()
{
	CPolyX *myPoly = new CPolyX;
	*myPoly = CreateLeapRoad ();// 1st polygon
	CPolyX *myPoly2 = new CPolyX;
	*myPoly2 = CreateTwoSegmentsLeap ();// 2nd polygon
	BOOL delta = false;
	CCoord* myCoord = new CCoord;// punct de intersectie ce va fi calculat
	
	int rez;// -1 - nu se intersecteaza;
		   //   1 - se intersecteaza
	
	rez = myPoly->IntWith(myPoly2, myCoord, delta);

	if( 1 == rez)
		ATLTRACE ( "(LEAP) Se intersecteaza la x = %f, y = %f\n", myCoord->x, myCoord->y);
	else
		ATLTRACE ("Nu se intersecteaza\n");
 
	delete(myPoly);
	delete(myPoly2);
	delete(myCoord);
}

void TestOffsetPerformance()
{
	CPolyX myPoly = CreateLeapRoad();
	CPolyX myNewPoly;

	CurveVectorPtr myCurveVector = CreateBentleyRoad();
	CurveVectorPtr myOffsetCurveVector;

	for (int g = 0; g < 2; g++)
	{
		CPerfTimer perfTimer;
		perfTimer.Start();
		int j = -5000;
		for (int i = 0; i < 10000; i++)
		{
			if (0 == g)
			{
				myPoly.Offset(&myNewPoly, j++);
			}
			else
			{
				myOffsetCurveVector = myCurveVector->CloneOffsetCurvesXY(CurveOffsetOptions::CurveOffsetOptions(j++));
			}
		}
		perfTimer.Stop();
		
		switch(g)
		{
		case 0:
			ATLTRACE ("Time elapsed LEAP: %5.2f\n", perfTimer.Elapsed());
			break;
		case 1:
			ATLTRACE ("Time elapsed Bentley: %5.2f\n", perfTimer.Elapsed());
			break;
		}

		//ATLTRACE ("Time elapsed %d: %5.2f\n", g+1, perfTimer.Elapsed());
	}
}

void TestIntLeapBentleyPerf()
{
	CurveLocationDetail location1, location2;
	CurveVectorPtr myPoly1B = CreateBentleyRoad();
	CurveVectorPtr myPoly2B = CreateTwoSegmentsBentley();
	
	CPolyX *myPolyL = new CPolyX;
	*myPolyL = CreateLeapRoad ();// 1st polygon
	CPolyX *myPoly2L = new CPolyX;
	*myPoly2L = CreateTwoSegmentsLeap ();// 2nd polygon
	BOOL delta = false;
	CCoord* myCoord = new CCoord;// punct de intersectie ce va fi calculat
	
	int rez;


	/*CurveCurve::ClosestApproach(location1, location2, *myPoly1B, *myPoly2B);
	rez = myPolyL->IntWith(myPoly2L, myCoord, delta);

	DPoint3d myPt1, myPt2;
	myPt1 = location1.point;
	myPt2 = location2.point;

	double toleranta = 1.0e-4;
	ASSERT (SameAs (myPt1.x, myCoord->x, toleranta));
	ASSERT (SameAs (myPt1.y, myCoord->y, toleranta));

	ATLTRACE ( "(LEAP) Se intersecteaza la x = %f, y = %f\n", myCoord->x, myCoord->y);
	ATLTRACE( "(BENTLEY) Se intersecteaza la x = %f, y = %f", myPt1.x, myPt1.y);*/
	
	CurveVectorPtr curveA = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);  
	CurveVectorPtr curveB = CurveVector::Create (CurveVector::BOUNDARY_TYPE_Open);
	DMatrix4d  worldToLocal;
	worldToLocal.initIdentity();
	
	//CurveCurve::IntersectionsXY(*curveA, *curveB, *myPoly1B, *myPoly2B, &worldToLocal);
	//
	//DPoint3d startPoint, endPoint;
	//curveA->GetStartEnd(startPoint, endPoint);
	//curveB->GetStartEnd(startPoint, endPoint);

	for (int g = 0; g < 2; g++)
	{
		CPerfTimer perfTimer;
		perfTimer.Start();
		for (int i = 0; i < 100000; i++)
		{
			if (0 == g)
			{
				rez = myPolyL->IntWith(myPoly2L, myCoord, delta);
			}
			else
			{
				//status = CurveCurve::ClosestApproach(location1, location2, *myPoly1B, *myPoly2B);
				CurveCurve::IntersectionsXY(*curveA, *curveB, *myPoly1B, *myPoly2B, &worldToLocal);
			}
		}
		perfTimer.Stop();

		switch(g)
		{
		case 0:
			ATLTRACE ("Time elapsed LEAP: %5.2f\n", perfTimer.Elapsed());
			break;
		case 1:
			ATLTRACE ("Time elapsed Bentley: %5.2f\n", perfTimer.Elapsed());
			break;
		}

		//ATLTRACE ("Time elapsed %d: %5.2f\n", g+1, perfTimer.Elapsed());
	}
}

void TestLeapAndBentleyLengths()
{
	CPolyX leapTest = CreateLeapRoad();
	CurveVectorPtr bentleyTest = CreateBentleyRoad();
	
	double lungimeLeap2D, lungimeLeap3D, lungimeRealaLeap, lungimeBentley, lungimeBentleyFast;//, lungimeRealaBentley;
	
	lungimeLeap2D = leapTest.Length2D();
	lungimeLeap3D = leapTest.Length3D();
	lungimeRealaLeap = leapTest.RealLength3D();

	lungimeBentley = bentleyTest->Length();
	lungimeBentleyFast = bentleyTest->FastLength();
	//lungimeRealaBentley = BentleyRealLength3D(bentleyTest);

	double toleranta = 1.0e-3f;

	//ASSERT (SameAs (lungimeRealaLeap, lungimeRealaBentley, toleranta ));
	
	TRACE ("LugimeLeap2D \t LugimeLeap3D \t LungimeRealLeap\n");
	TRACE ("%f \t %f \t %f\n", lungimeLeap2D, lungimeLeap3D, lungimeRealaLeap);

	TRACE ("LungimeBentley \t LungimeBentleyFast\n");
	TRACE ("%f \t  %f", lungimeBentley, lungimeBentleyFast);

	ASSERT (SameAs (lungimeLeap2D, lungimeBentley, toleranta ));
	ASSERT (SameAs (lungimeLeap2D, lungimeBentleyFast, toleranta));

	ASSERT (SameAs (lungimeLeap3D, lungimeBentley, toleranta));
	ASSERT (SameAs (lungimeLeap3D, lungimeBentleyFast, toleranta));

	TRACE (" ");
}

/// <summary>
/// Tests the set off alg_ performance.
/// </summary>
void TestSetOffAlg_Performance()
{
	CurveVectorPtr curveVector = CreateBentleyRoad();
	CPolyX leapRoad = CreateLeapRoad();

	SetupCurveData(curveVector);

	double tolerance = 1e-1f;

	CCoord coord;
	DPoint3d point;

	// setoffalg
	double x = 0.0e0;
	double y = 0.0e0;
	double offset, along;

	for (int g = 0; g < 2; g++)
	{
		along = 0;
		CPerfTimer perfTimer;
		perfTimer.Start();
		for (int i = 0; i < 3200; i++)
		{
			along += 0.5f;
			offset = -500;
			for (int j = 0; j < 1600; j++)
			{
				offset += 0.5f;
				if (0 == g)
					leapRoad.SetOffAlg (&coord, offset, along);
				else
					BentleySetOffAlg (curveVector, point, offset, along);
			}
		}
		perfTimer.Stop();

		TRACE ("Time elapsed %d: %5.2f\n", g+1, perfTimer.Elapsed());
	}
}
 
/// <summary>
/// Tests the set off alg_ values.
/// </summary>
void TestSetOffAlg_Values()
{

	CurveVectorPtr curveVector = CreateBentleyRoad();
	CPolyX leapRoad = CreateLeapRoad();

	SetupCurveData(curveVector);

	double tolerance = 1e-3f;

	// setoffalg
	// double x, y;
	double offset, along;

	CCoord coord;
	DPoint3d point;
	along = 0;

	CPerfTimer perfTimer;
	perfTimer.Start();
	for (int i = 0; i < 3200; i++)
	{
		along += 0.5f;
		offset = -500;
		for (int j = 0; j < 1600; j++)
		{
			offset += 0.5f;
			leapRoad.SetOffAlg (&coord, offset, along);
			BentleySetOffAlg (curveVector, point, offset, along);

			ASSERT (SameAs (point.x, coord.x, tolerance));
			ASSERT (SameAs (point.y, coord.y, tolerance));
		}
	}
	perfTimer.Stop();

	TRACE ("Time elapsed: %5.2f\n", perfTimer.Elapsed());
}

/// <summary>
/// Tests the get off alg_ values.
/// </summary>
void TestGetOffAlg_Values()
{
	//CurveVectorPtr curveVector = CreateBentleyRoad();
	//CPolyX leapRoad = CreateLeapRoad();
	
	CurveVectorPtr curveVector = CreateTriangleModelB();
	CPolyX leapRoad = CreateTriangleModelL();

	SetupCurveData(curveVector);

	double tolerance = 1.0e-1f;

	// getoffalg
	double offsetL, offsetB,  alongL, alongB;

	double x = 10;
	double y = -12;
	CCoord coord(x, y);

	DPoint3d point;
	point.init(x, y);

	{
		leapRoad.GetOffAlg (&coord, &offsetL, &alongL);
		BentleyGetOffAlg (curveVector, point, &offsetB, &alongB);
		DPoint3d ptB;
		CCoord ptL;
		leapRoad.SetOffAlg (&ptL, offsetL, alongL);
		BentleySetOffAlgOld (curveVector, ptB, offsetB, alongB);
		ATLTRACE("");
	}

	for(int i = 0; i < 50; i++)
	{
		x += 5.0e-1;
		y = -45;
		for(int j = 0; j < 180; j++)
		{
			y += 5.0e-1;
			coord.x = x;
			coord.y = y;
			point.x = x; 
			point.y = y;
			leapRoad.GetOffAlg (&coord, &offsetL, &alongL);
			BentleyGetOffAlg (curveVector, point, &offsetB, &alongB);

			ASSERT (SameAs (offsetL, offsetB));
			ASSERT (SameAs (alongL, alongB));
		}
	}

	CPerfTimer perfTimer;
	perfTimer.Start();
	for (int i = 0; i < 3200; i++)
	{
		x += 5.0e-1;
		y = -100;
		for (int j = 0; j < 1600; j++)
		{
			y += 5.0e-1;
			coord.x = x;
			coord.y = y;
			point.x = x; 
			point.y = y;
			leapRoad.GetOffAlg (&coord, &offsetL, &alongL);
			BentleyGetOffAlg (curveVector, point, &offsetB, &alongB);

			ASSERT (SameAs (offsetL, offsetB, tolerance));
			ASSERT (SameAs (alongL, alongB, tolerance));
		}
	}
	perfTimer.Stop();

	TRACE ("Time elapsed: %5.2f\n", perfTimer.Elapsed());
}

/// <summary>
/// Tests the get off alg_ performance.
/// </summary>
void TestGetOffAlg_Performance()
{
	CurveVectorPtr curveVector = CreateBentleyRoad();
	CPolyX leapRoad = CreateLeapRoad();

	SetupCurveData(curveVector);

	double tolerance = 1.0e-1f;

	double x = 10;
	double y = -5;
	CCoord coord(x, y);
	DPoint3d point;
	point.init(x, y);

	// getoffalg
	double offsetL , offsetB,  alongL, alongB;

	for (int g = 0; g < 2; g++)
	{
		//along = 0;
		CPerfTimer perfTimer;
		perfTimer.Start();
		for (int i = 0; i < 1600000; i++)
		{
			x += 0.1;
			y += 0.01;
			if (0 == g)
			{
				CCoord coord (x, y);
				leapRoad.GetOffAlg (&coord, &offsetL, &alongL);
			}
			else
			{
				BentleyGetOffAlg (curveVector, DPoint3d::From(x, y), &offsetB, &alongB);
			}
		}
		perfTimer.Stop();

		TRACE ("Time elapsed %d: %5.2f\n", g+1, perfTimer.Elapsed());
	}
}

/// <summary>
/// Called when [bn clicked button1].
/// </summary>
void CMFCAppDlg::OnBnClickedButton1()
{

#if 0
	TestSetOffAlg_Values();
#endif

#if 0
	TestSetOffAlg_Performance();
#endif

#if 1

	/*
	CPolyX		         polyL = CreateLeapRoad();
	CurveVectorPtr       polyB = CreateBentleyRoad();
	CBentleyLeapPolyX    test (polyL, polyB);
	CBentleyLeapPolyX    test2;

	CBentleyLeapPolyX::Dump(&test2.m_polyLeap, test2.m_polyBentley);
	CBentleyLeapPolyX::Dump(&polyL, polyB);
	
	double lungimeL = test.m_polyLeap.Length2D();
	double lungimeB = test.m_polyBentley->FastLength();

	test.Add(CCourse(CCoord(0, 0), CCoord(0, 20)), DSegment3d::From(0, 0, 0, 0, 20, 0));
	
	lungimeL = test.m_polyLeap.Length2D();
	lungimeB = test.m_polyBentley->FastLength();

	CBentleyLeapPolyX::Dump(&test.m_polyLeap, test.m_polyBentley);

	ATLTRACE ("");*/

	CPolyXBentley myPoly;// = new CPolyXBentley;
	CurveVectorPtr myVec, myVec2;
	
	myPoly.AddEqual(CreateBentleyRoad());
	myPoly.Reverse();
	myVec = myPoly.Add(CreateTwoSegmentsBentley());
	myVec2 = myVec->Clone();
	
	BOOL isSame = false;

	isSame = myVec->IsSameStructure(*myVec2);

	//myPoly.AddEqual(CreateTwoSegmentsBentley());
	//myVec = myPoly.Add(CreateBentleyRoad());

	double lungime1 = myPoly.Length2D();

	BOOL rez = false;

	MSBsplineCurvePtr bsplineCurveptr = myVec2->GetBsplineCurve();

	double lung = myVec->FastLength();

	if(lung > 0.0e0)
		rez = true;

	ATLTRACE ("Fraction to point for Bentley .DLL\n");
	for (int i = 0; i <= lung ; i++)
	{
		DPoint3d xyz;
		bsplineCurveptr->FractionToPoint (xyz, (1.0f / 20) * i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");

	rez = myPoly.MyDump(myPoly.m_curve);

	//TestGetOffAlg_Values();
#endif	

#if 0
	TestGetOffAlg_Performance();
#endif	

#if 0
	MyDumpPoly ();
#endif	

#if 0
	MyDumpBentley ();
#endif

#if 0
	CreateMySpiral();
#endif

#if 0
	//TestLeapAndBentleyLengths();
	TestCreateSegmentsBentley();
#endif

#if 0
	TestLeapIntWith ();
#endif

#if 0
	ClosestApproach();
#endif

#if 0
	TestOffsetLEAP();
#endif

#if 0
	TestOffsetBentley();
#endif

#if 0
	TestOffsetPerformance();
#endif

#if 0
	TestIntLeapBentleyPerf();
#endif

	ATLTRACE ("");
}

#if 0
void CreateStar()
{
	CCourse line1(CCoord(0, 75, 0), CCoord(25, 25, 0));
	CCourse line2(CCoord(25, 25, 0), CCoord(75, 0, 0));
	CCourse line3(CCoord(75, 0, 0), CCoord(25, -25, 0));
	CCourse line4(CCoord(25, -25, 0), CCoord(0, -75, 0));
	CCourse line5(CCoord(0, -75, 0), CCoord(-25, -25, 0));
	CCourse line6(CCoord(-25, -25, 0), CCoord(-75, 0, 0));
	CCourse line7(CCoord(-75, 0, 0), CCoord(-25, 25, 0));
	CCourse line8(CCoord(-25, 25, 0), CCoord(0, 75, 0));

	CPolyX poly;

	poly+=line1;
	poly+=line2;
	poly+=line3;
	poly+=line4;
	poly+=line5;
	poly+=line6;
	poly+=line7;
	poly+=line8;

	double lungime  = poly.Length3D();

	ATLTRACE ("Fraction to point\n");
	for (int i = 0; i <= lungime; i+=1)
	{
		CCoord xyz;
		poly.SetOffAlg(&xyz, 0, i);
		ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
	}
	ATLTRACE ("");

}

////CCoord pt1(0, 0, 0);
////CCoord pt2(0, 10, 0);

//CCourse line1(CCoord(0, 0, 0), CCoord(25, 0, 0));

//CCourse line2(CCoord(25, 0, 0), CCoord(25, 30, 0));

////CCourse line3(CCoord(25, 30, 0), CCoord(50, 30, 0));

//CCourse line4(CCoord(50, 30, 0), CCoord(50, 0, 0));

//CCourse line5(CCoord(50, 0, 0), CCoord(75, 0, 0));

//CCourse line6(CCoord(75, 0, 0), CCoord(75, 30, 0));

//CCourse line7(CCoord(75, 30, 0), CCoord(100, 30, 0));

////CArc arc1(pt2, CCoord(37.5, 12.5), CCoord(50.0, 50.0));

//CArc arc1(CCoord(25, 30, 0), CCoord(25, 35), CCoord(50.0, 30.0));
//
//poly+=line1;
//poly+=line2;
//poly+=arc1;
////poly+=line3;
//poly+=line4;
//poly+=line5;
//poly+=line6;
//poly+=line7;
////poly+=arc1;

// ATLTRACE ("Fraction to point\n");
//   for (int i = 0; i <= 200; i++)
//       {
//       CCoord xyz;
//       //bsplineCurve->FractionToPoint (xyz, (1.0f / 100) * i);
//	poly.SetOffAlg(&xyz, 0, i);
//       ATLTRACE ("%5.2f \t %5.2f\n", xyz.x, xyz.y);
//       }
//   ATLTRACE ("");

////CCourse line2(CCoord(50.0, 0.0),CCoord(75.0, 10.0));

////poly+=line2;

////CArc arc2(CCoord(75.0, 10.0), CCoord(75.0+12.5, -2.5), CCoord(100.0, 10.0));

////poly+=arc2;

//CCoord pta(-2.0, -10.0, 0.0);

////CCoord ptb(25.0, -10.0);

////CCoord ptc(37.5, -10.0);

//double myoff, myalg;

//bool mymessage = false;

//mymessage = poly.GetOffAlg(&pta,&myoff, &myalg); 



////cout << mymessage << " " << endl;

////cout << myoff << " " << endl;

////cout << myalg << " " << endl;


////mymessage = poly.GetOffAlg(&ptc,&myoff, &myalg); 

//////mymessage = arc1.GetOffAlg(&ptc,&myoff, &myalg);

////cout << mymessage << " " << endl;

////cout << myoff << " " << endl;

////cout << myalg << " " << endl;


////CCoord ptx; 

////poly.SetOffAlg(&ptx, 37.5, 44.635);

////cout << endl;


//for(int i = 0; i < poly.Length2D(); i++)
//{
//	CCoord ptx;
//	poly.SetOffAlg(&ptx, 0, i);
//
//	ATLTRACE ("%5.2f \t %5.2f\n", ptx.x, ptx.y);
//}
//
//cout << endl;
#endif
