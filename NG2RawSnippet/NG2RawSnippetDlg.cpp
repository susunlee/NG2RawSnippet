
// NG2RawSnippetDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "NG2RawSnippet.h"
#include "NG2RawSnippetDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	IMAGE_WIDTH					200
#define	IMAGE_HEIGHT				200
#define IMAGE_ITEMS_PER_LINE		10

#define WIDTHBYTES(bits)   (((bits)+31)/32*4)  // 4바이트 배수

void PumpMessage()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNG2RawSnippetDlg 대화 상자



CNG2RawSnippetDlg::CNG2RawSnippetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NG2RAWSNIPPET_DIALOG, pParent)
	, m_strPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNG2RawSnippetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPath);
	DDX_Control(pDX, IDC_LIST1, m_ctlList);
	DDX_Control(pDX, IDC_STATIC_PIC, m_ctlPIC);
	DDX_Control(pDX, IDC_STATIC_INFO, m_ctlTitle);
	DDX_Control(pDX, IDC_CHECK1, m_cbShowImage);
	DDX_Control(pDX, IDC_LIST2, m_ctlNGItem);
	DDX_Control(pDX, IDC_EDIT2, m_ctlCSV);
	DDX_Control(pDX, IDC_STATIC_MASTER, m_ctlMaster);
	DDX_Control(pDX, IDC_STATIC_SCAN, m_ctlScan);
	DDX_Control(pDX, IDC_EDIT_TOT_NG, m_ctlTotalNG);
	DDX_Control(pDX, IDC_EDIT_TOT_GOOD, m_ctlAIGood);
	DDX_Control(pDX, IDC_EDIT_RATE, m_ctlAIGoodRate);
}

BEGIN_MESSAGE_MAP(CNG2RawSnippetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CNG2RawSnippetDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CNG2RawSnippetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_AI_RESULT, &CNG2RawSnippetDlg::OnBnClickedButtonAiResult)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)	
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickNGItem)
	ON_BN_CLICKED(IDC_BUTTON_NG, &CNG2RawSnippetDlg::OnBnClickedButtonNg)
	ON_BN_CLICKED(IDC_BUTTON_NG2, &CNG2RawSnippetDlg::OnBnClickedButtonNg2)
END_MESSAGE_MAP()

// CNG2RawSnippetDlg 메시지 처리기

BOOL CNG2RawSnippetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect r;
	m_ctlList.GetClientRect(&r);

	m_ctlList.InsertColumn(0, _T("번호"), LVCFMT_CENTER, 100);
	m_ctlList.InsertColumn(1, _T("파일명"), LVCFMT_CENTER, r.Width() - 600);
	m_ctlList.InsertColumn(2, _T("NG 갯수"), LVCFMT_CENTER, 150);
	m_ctlList.InsertColumn(3, _T("변환결과"), LVCFMT_CENTER, 150);
	m_ctlList.InsertColumn(4, _T("AI Good 갯수"), LVCFMT_CENTER, 150);
	m_ctlList.InsertColumn(5, _T("파일경로"), LVCFMT_CENTER, 0);
	m_ctlList.InsertColumn(6, _T("AI Result"), LVCFMT_CENTER, 0);

	m_ctlNGItem.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctlNGItem.InsertColumn(0, _T("번호"), LVCFMT_CENTER, 90);
	m_ctlNGItem.InsertColumn(1, _T("파일명"), LVCFMT_CENTER, 350);
	m_ctlNGItem.InsertColumn(2, _T("NG"), LVCFMT_CENTER, 90);
	m_ctlNGItem.InsertColumn(3, _T("파일경로"), LVCFMT_CENTER, 0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CNG2RawSnippetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNG2RawSnippetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNG2RawSnippetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


static int CALLBACK BrowseForFolder_CallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
	}

	return 0;
}


void CNG2RawSnippetDlg::OnBnClickedButton1()
{
	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = NULL;
	BrInfo.pidlRoot = NULL;
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowseForFolder_CallbackProc;
	BrInfo.lParam = (LPARAM)(LPCSTR)(LPCTSTR)m_strPath;

	BrInfo.lpszTitle = _T("NG Result Image 폴더를 선택하세요");


	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);				// 파일경로 읽어오기

	if (0 < _tcslen(szBuffer))
	{
		m_strPath.Format(_T("%s"), szBuffer);

		UpdateData(FALSE);

		SetFileList();
	}
}

void CNG2RawSnippetDlg::DrawRawDataIntoDC(CDC* pDC, BYTE* pViewImg, int x, int y, int width, int height, int bitCount, int ai_result_show /*= FALSE */)
{
	BITMAPINFO *pBmpInfo = NULL;
	if (pBmpInfo != NULL)
	{
		delete[] pBmpInfo;
		pBmpInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD))];
	}
	else
		pBmpInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD))];


	pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmpInfo->bmiHeader.biPlanes = 1;
	pBmpInfo->bmiHeader.biCompression = 0; //BI_RGB;// 0;  
	pBmpInfo->bmiHeader.biWidth = width; //이미지 가로 크기를 적음.  
	pBmpInfo->bmiHeader.biHeight = height;
	pBmpInfo->bmiHeader.biBitCount = bitCount;
	pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biClrUsed = 256;
	pBmpInfo->bmiHeader.biClrImportant = 0;//SHRT_MAX;  
	pBmpInfo->bmiHeader.biSizeImage = height * WIDTHBYTES(width * bitCount);

	if (bitCount == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo->bmiColors[i].rgbBlue = i;
			pBmpInfo->bmiColors[i].rgbGreen = i;
			pBmpInfo->bmiColors[i].rgbRed = i;
			pBmpInfo->bmiColors[i].rgbReserved = 0;
		}
	}


	//pViewImg 가 RAW DATA
	if (pViewImg != NULL)
	{
		//화면에 출력하기 전에 메모리에 먼저 적재한다. 또한, 비트맵 구조체를 만든다.  
		CRect rect;
		CDC memDC;
		CBitmap bitMap, *pOldBitmap;

		// 호환되는 memDC를 메모리에 생성한다.  
		memDC.CreateCompatibleDC(pDC);
		bitMap.CreateCompatibleBitmap(pDC, width, height);
		pOldBitmap = (CBitmap*)memDC.SelectObject(&bitMap);
		memDC.SelectObject(&bitMap);
		memDC.FillSolidRect(&rect, RGB(255, 255, 255));

		// image 역상

		int rwsize = WIDTHBYTES(width * bitCount);
		for (int i = 0; i < height / 2; i++) {
			for (int j = 0; j < width * 3; j++) {
				char temp = pViewImg[i*rwsize + j];
				pViewImg[i*rwsize + j] = pViewImg[(height - i - 1)* rwsize + j];
				pViewImg[(height - i - 1)* rwsize + j] = temp;
			}
		}


		SetDIBitsToDevice(
			memDC.GetSafeHdc(), // Handle to the device context.  
			0, 0, // Specifies the x and y-coordinate, in logical units, of the upper-left corner of the destination rectangle.  
			width, height, //가로, 세로를 적는다.  
			// Specifies the width and height, in logical units, of the DIB.  
			0, 0, // Specifies the x and y-coordinate, in logical units, of the lower-left corner of the DIB.  
			0, // Specifies the starting scan line in the DIB.  
			height, //세로를 적음. // Specifies the number of DIB scan lines contained in the array pointed to by the lpvBits parameter.  
			pViewImg, pBmpInfo, DIB_RGB_COLORS);



		//StretchBltMode 설정.  
		pDC->SetStretchBltMode(COLORONCOLOR);

		//dc.BitBlt(0, 0, rectLP.right, rectLP.bottom, &memDC,0, 0, SRCCOPY); 

		if (ai_result_show == TRUE)
		{
			pDC->StretchBlt(0, 0, width * 2, height * 2, &memDC, 0, 0, width, height, SRCCOPY);   //크기조절 가능하다.
		}
		else
		{
			pDC->BitBlt(x, y, width, height, &memDC, 0, 0, SRCCOPY);
		}
	}


	if (pBmpInfo != NULL)
	{
		delete[] pBmpInfo;
		pBmpInfo = NULL;
	}
}


void CNG2RawSnippetDlg::OnBnClickedOk()
{
	/*{
		CDC* hDC = GetDesktopWindow()->GetWindowDC();
		FILE *fp;
		BYTE *f = (BYTE*)malloc(200 * 3 * 200);
	
		fopen_s(&fp, "D:\\Share\\Drive\\Result\\err2\\not_detected_image\\[00006] 000001_02_master.raw", "rb");	
		fread(f, 1, 200 * 3 * 200, fp);
	
		DrawRawDataIntoDC(hDC, f, 0, 0, 200, 200, 24, TRUE);
	
		free(f);	
		fclose(fp);
		ReleaseDC(hDC);
	
		return;
	}*/

	UpdateData();

	CDC* hDC = GetDesktopWindow()->GetWindowDC();

	for (int ii = 0; ii < m_ctlList.GetItemCount(); ii++)
	{
		int ng_items;
		CImage image_scan, image_master;
		CString path = m_ctlList.GetItemText(ii, 5);

		m_ctlList.SetItemState(-1, ii, LVIS_SELECTED | LVIS_FOCUSED);
		m_ctlList.SetItemState(ii, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_ctlList.EnsureVisible(ii, FALSE);
		m_ctlList.SetFocus();

		// NG 스캔 이미지
		{
			path.Replace(_T(".csv"), _T("_defect.jpg"));

			ng_items = _ttoi(m_ctlList.GetItemText(ii, 2));

			if (ng_items == 0)
			{
				m_ctlList.SetItemText(ii, 3, _T("PASS"));
				PumpMessage();
				continue;
			}

			CFileStatus status;
			if (CFile::GetStatus(path, status))
			{
			}
			else 
			{
				m_ctlList.SetItemText(ii, 3, _T("No File"));
				PumpMessage();
				continue;
			}

			m_ctlList.SetItemText(ii, 3, _T("변환시작"));
			PumpMessage();

			image_scan.Load(path);

			CString msg;
			BYTE* tmp = NULL;
			BYTE* src = NULL;
			BYTE* src_save = NULL;
			BYTE* src1 = (BYTE*)image_scan.GetBits();
			int p = image_scan.GetPitch();
			int h = image_scan.GetHeight();


			if (p < 0)
			{
				src1 += (p * (h - 1));
			}

			src = (BYTE*)malloc(abs(p) * h);
			src_save = src;

			for (int i = (h - 1); -1 < i; i--)
			{
				memcpy(src + (i * abs(p)), src1, abs(p));

				src1 += abs(p);
			}

			//전체 이미지 저장
			//tmp = (BYTE*)malloc(abs(p) * h);
			//memcpy(tmp, src, abs(p) * h);
			//DrawRawDataIntoDC(hDC, tmp, 0, 0, IMAGE_WIDTH * IMAGE_ITEMS_PER_LINE, h, image.GetBPP());
			//free(tmp);
			//ReleaseDC(hDC);
			//return;

			tmp = (BYTE*)malloc(IMAGE_WIDTH * 3 * IMAGE_HEIGHT);

			for (int i = 0, k = 0; i < ng_items/*(h / IMAGE_HEIGHT * IMAGE_ITEMS_PER_LINE)*/; i++, k++)
			{
				for (int j = 0; j < IMAGE_HEIGHT; j++)
				{
					memcpy(tmp + (j * (IMAGE_WIDTH * 3)), src + (j * abs(p)) + ((k % IMAGE_ITEMS_PER_LINE) * IMAGE_WIDTH * 3), IMAGE_WIDTH * 3);
				}

				CString temp_path = path;

				msg.Format(_T("defect_image\\[%05d] "), i + 1);
				temp_path.Insert(m_strPath.GetLength() + 1, msg);
				temp_path.Replace(_T(".jpg"), _T(".raw"));

				CT2CA szPath(temp_path, CP_ACP);
				FILE *fp = NULL;
				fopen_s(&fp, szPath, "wb");
				fwrite(tmp, 1, (IMAGE_WIDTH * 3 * IMAGE_HEIGHT), fp);
				fclose(fp);

				if (m_cbShowImage.GetCheck())
				{
					DrawRawDataIntoDC(hDC, tmp, (i % IMAGE_ITEMS_PER_LINE) * IMAGE_WIDTH, (i / IMAGE_ITEMS_PER_LINE) * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, image_scan.GetBPP());
				}

				//src += (200 * 3);

				if ((i > 0) && ((i % IMAGE_ITEMS_PER_LINE) == 9))
				{
					src += (abs(p) * IMAGE_WIDTH);
				}
			}

			free(tmp);
			free(src_save);
		}

		// NG 마스터 이미지
		{
			path.Replace( _T("_defect.jpg"), _T("_master.jpg"));

			CFileStatus status;
			if (CFile::GetStatus(path, status))
			{
			}
			else
			{
				m_ctlList.SetItemText(ii, 3, _T("No File"));
				PumpMessage();
				continue;
			}

			image_master.Load(path);

			CString msg;
			BYTE* tmp = NULL;
			BYTE* src = NULL;
			BYTE* src_save = NULL;
			BYTE* src1 = (BYTE*)image_master.GetBits();
			int p = image_master.GetPitch();
			int h = image_master.GetHeight();


			if (p < 0)
			{
				src1 += (p * (h - 1));
			}

			src = (BYTE*)malloc(abs(p) * h);
			src_save = src;

			for (int i = (h - 1); -1 < i; i--)
			{
				memcpy(src + (i * abs(p)), src1, abs(p));

				src1 += abs(p);
			}


			tmp = (BYTE*)malloc(IMAGE_WIDTH * 3 * IMAGE_HEIGHT);

			for (int i = 0, k = 0; i < ng_items; i++, k++)
			{
				for (int j = 0; j < IMAGE_HEIGHT; j++)
				{
					memcpy(tmp + (j * (IMAGE_WIDTH * 3)), src + (j * abs(p)) + ((k % IMAGE_ITEMS_PER_LINE) * IMAGE_WIDTH * 3), IMAGE_WIDTH * 3);
				}

				CString temp_path = path;
				msg.Format(_T("master_image\\[%05d] "), i + 1);
				temp_path.Insert(m_strPath.GetLength() + 1, msg);
				temp_path.Replace(_T(".jpg"), _T(".raw"));

				CT2CA szPath(temp_path, CP_ACP);
				FILE *fp = NULL;
				fopen_s(&fp, szPath, "wb");
				fwrite(tmp, 1, (IMAGE_WIDTH * 3 * IMAGE_HEIGHT), fp);
				fclose(fp);

				//DrawRawDataIntoDC(hDC, tmp, (i % IMAGE_ITEMS_PER_LINE) * IMAGE_WIDTH, (i / IMAGE_ITEMS_PER_LINE) * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, image_master.GetBPP());

				if ((i > 0) && ((i % IMAGE_ITEMS_PER_LINE) == 9))
				{
					src += (abs(p) * IMAGE_WIDTH);
				}
			}

			free(tmp);
			free(src_save);
		}

		m_ctlList.SetItemText(ii, 3, _T("변환종료"));
		PumpMessage();
	}

	ReleaseDC(hDC);

	AfxMessageBox(_T("변환종료"));

	//image.Save(_T("d:\\temp.bmp"), Gdiplus::ImageFormatBMP);
}


int CNG2RawSnippetDlg::GetNGCount(CString f_name)
{
	int items = 0;
	CString temp;
	CStdioFile file(f_name, CFile::modeRead | CFile::typeText);

	while (1)
	{
		if (file.ReadString(temp))
		{
			items++;
		}
		else
		{
			break;
		}
	}

	file.Close();

	return items - 11;
}

int CNG2RawSnippetDlg::GetNGCount2(CString f_name, CString* ai_result)
{
	int items = 0, good_count = 0;
	CString temp, msg;
	CStdioFile file(f_name, CFile::modeRead | CFile::typeText);

	while (1)
	{
		if (file.ReadString(temp))
		{
			items++;

			if (11 < items)
			{
				int index1 = 0, index2 = 0, comma_cnt = 0;
				int temp_len = temp.GetLength();

				for (index1 = 0; index1 < temp_len; index1++)
				{
					if (temp.GetAt(index1) == _T(','))
					{
						comma_cnt++;
						if (comma_cnt == 12)		// 13 컬럼에 AI 결과 저장
						{
							break;
						}
					}
				}

				index2 = temp.Find(_T(','), index1 + 1);

				if (index2 < 0)
				{
					msg = temp.Mid(index1 + 1);
				}
				else
				{
					msg = temp.Mid(index1 + 1, index2 - index1 - 1);
				}

				if (_ttoi(msg) == 1)
				{
					good_count++;
				}

				*ai_result += msg;
				*ai_result += _T(",");
			}
		}
		else
		{
			break;
		}
	}

	file.Close();

	return good_count;
}

void CNG2RawSnippetDlg::SetFileList()
{
	CFileFind ff;
	CString csv_path, temp;
	BOOL is_ok = FALSE;
	int ng_count;

	m_nItems = 0;
	m_ctlList.DeleteAllItems();

	csv_path.Format(_T("%s\\*.csv"), m_strPath);
	//csv_path.Format(_T("%s\\*.csv"), _T("D:\\Share\\Drive\\Result\\NW0257-A2B\\AWR-57273237"));
	is_ok = ff.FindFile(csv_path);


	m_nTotalImage = 0;

	while (is_ok)
	{
		is_ok = ff.FindNextFile();

		if (ff.IsArchived())
		{
			temp.Format(_T("%d"), m_nItems + 1);
			m_ctlList.InsertItem(m_nItems, temp);
			m_ctlList.SetItemText(m_nItems, 1, ff.GetFileName());

			ng_count = GetNGCount(ff.GetFilePath());
			m_nTotalImage += ng_count;

			temp.Format(_T("%d"), ng_count);
			m_ctlList.SetItemText(m_nItems, 2, temp);
			m_ctlList.SetItemText(m_nItems, 5, ff.GetFilePath());

			m_nItems++;

			PumpMessage();
		}
	}

	if (0 < m_nTotalImage)
	{
		temp.Format(_T("%s\\defect_image"), m_strPath);
		CreateDirectory(temp, NULL);
		temp.Format(_T("%s\\master_image"), m_strPath);
		CreateDirectory(temp, NULL);
		temp.Format(_T("%s\\not_detected_image"), m_strPath);
		CreateDirectory(temp, NULL);
		temp.Format(_T("%s\\no_defected_image"), m_strPath);
		CreateDirectory(temp, NULL);
	}

	temp.Format(_T("NG 파일 정보 (총 CSV 파일수: %d)"), m_nItems);
	m_ctlTitle.SetWindowText(temp);

	temp.Format(_T("%d"), m_nTotalImage);
	m_ctlTotalNG.SetWindowText(temp);
}

void CNG2RawSnippetDlg::OnBnClickedButtonAiResult()
{
	int items = m_ctlList.GetItemCount();
	CString temp, msg;
	int good_count = 0, good_tot_count = 0;

	for (int i = 0; i < items; i++)
	{
		msg.Empty();
		good_count = GetNGCount2(m_ctlList.GetItemText(i, 5), &msg);
		temp.Format(_T("%d"), good_count);

		good_tot_count += good_count;

		m_ctlList.SetItemText(i, 4, temp);
		m_ctlList.SetItemText(i, 6, msg);
	}

	temp.Format(_T("%d"), good_tot_count);
	m_ctlAIGood.SetWindowText(temp);

	m_ctlTotalNG.GetWindowText(temp);

	float rate = (float)good_tot_count / (float)m_nTotalImage * 100.;
	temp.Format(_T("%3.3f%%"), rate);
	m_ctlAIGoodRate.SetWindowText(temp);
}

void CNG2RawSnippetDlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pNMHDR;
	int ng_items = _ttoi(m_ctlList.GetItemText(pnmlv->iItem, 2));
	CString temp = m_ctlList.GetItemText(pnmlv->iItem, 1), temp1, msg;
	CString ai_result = m_ctlList.GetItemText(pnmlv->iItem, 6);

	if (m_ctlList.GetItemText(pnmlv->iItem, 4).GetLength() < 1)
	{
		return;
	}

	m_ctlCSV.SetWindowText(temp);
	m_ctlNGItem.DeleteAllItems();

	for (int i = 0; i < ng_items; i++)
	{
		temp.Replace(_T(".csv"), _T("_defect.raw"));
		msg.Format(_T("%s\\defect_image\\[%05d] %s"), m_strPath, i + 1, temp);

		temp1.Format(_T("%d"), i + 1);
		m_ctlNGItem.InsertItem(i, temp1);
		temp1.Format(_T("[%05d] %s"), i + 1, temp);
		m_ctlNGItem.SetItemText(i, 1, temp1);
		AfxExtractSubString(temp1, ai_result, i, _T(','));
		m_ctlNGItem.SetItemText(i, 2, temp1);
		m_ctlNGItem.SetItemText(i, 3, msg);

		PumpMessage();
	}
}

void CNG2RawSnippetDlg::OnClickNGItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pNMHDR;

	ShowImage(pnmlv->iItem);
}

void CNG2RawSnippetDlg::ShowImage(int index)
{
	CWnd* wnd_scan = GetDlgItem(IDC_STATIC_SCAN);
	CWnd* wnd_master = GetDlgItem(IDC_STATIC_MASTER);
	CDC* dc_scan = wnd_scan->GetWindowDC();
	CDC* dc_master = wnd_master->GetWindowDC();

	CString raw_path = m_ctlNGItem.GetItemText(index, 3);
	BYTE* pViewImg = NULL;
	size_t size = 0;
	int retry_item = 0;
	
__try_again:

	{
		CT2CA szPath(raw_path, CP_ACP);
		FILE *fp = NULL;
		fopen_s(&fp, szPath, "rb");

		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			pViewImg = (BYTE*)malloc(size);
			fread(pViewImg, 1, size, fp);
			fclose(fp);

			DrawRawDataIntoDC((retry_item == 0) ? dc_scan : dc_master, pViewImg, 0, 0, 200, 200, 24, TRUE);

			free(pViewImg);
			pViewImg = NULL;
		}

		raw_path.Replace(_T("\\defect_image"), _T("\\master_image"));
		raw_path.Replace(_T("_defect.raw"), _T("_master.raw"));


		if (++retry_item == 1)
		{
			goto __try_again;
		}
	}


	dc_scan->DeleteDC();
	dc_master->DeleteDC();
}


void CNG2RawSnippetDlg::OnBnClickedButtonNg()
{
	POSITION pos = m_ctlNGItem.GetFirstSelectedItemPosition();
	int index = m_ctlNGItem.GetNextSelectedItem(pos);

	CString path, temp;
	temp = path = m_ctlNGItem.GetItemText(index, 3);

	temp.Replace(_T("\\defect_image"), _T("\\not_detected_image"));
	CopyFile(path, temp, FALSE);

	path.Replace(_T("\\defect_image"), _T("\\master_image"));
	path.Replace(_T("_defect.raw"), _T("_master.raw"));

	temp = path;

	temp.Replace(_T("\\master_image"), _T("\\not_detected_image"));
	CopyFile(path, temp, FALSE);
}


void CNG2RawSnippetDlg::OnBnClickedButtonNg2()
{
	POSITION pos = m_ctlNGItem.GetFirstSelectedItemPosition();
	int index = m_ctlNGItem.GetNextSelectedItem(pos);

	CString path, temp;
	temp = path = m_ctlNGItem.GetItemText(index, 3);

	temp.Replace(_T("\\defect_image"), _T("\\no_defected_image"));
	CopyFile(path, temp, FALSE);

	path.Replace(_T("\\defect_image"), _T("\\master_image"));
	path.Replace(_T("_defect.raw"), _T("_master.raw"));

	temp = path;

	temp.Replace(_T("\\master_image"), _T("\\no_defected_image"));
	CopyFile(path, temp, FALSE);
}


BOOL CNG2RawSnippetDlg::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pWnd = GetFocus();

	if (pWnd)
	{
		if (pWnd->m_hWnd == m_ctlNGItem.m_hWnd)
		{
			if (pMsg->message == WM_KEYUP)
			{
				if ((pMsg->wParam == VK_UP) || (pMsg->wParam == VK_DOWN))
				{
					POSITION pos = m_ctlNGItem.GetFirstSelectedItemPosition();
					int index = m_ctlNGItem.GetNextSelectedItem(pos);

					ShowImage(index);

					return FALSE;
				}
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
