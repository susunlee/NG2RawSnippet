
// NG2RawSnippetDlg.h: 헤더 파일
//

#pragma once


// CNG2RawSnippetDlg 대화 상자
class CNG2RawSnippetDlg : public CDialogEx
{
// 생성입니다.
public:
	CNG2RawSnippetDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void	SetFileList();
	int		GetNGCount(CString f_name);
	int		GetNGCount2(CString f_name, CString* ai_result);
	void	DrawRawDataIntoDC(CDC* pDC, BYTE* pViewImg, int x, int y, int width, int height, int bitCount, int ai_result_show = FALSE);
	void	ShowImage(int index);

public:
	CString		m_strPath;
	CListCtrl	m_ctlList;
	int			m_nItems;
	int			m_nTotalImage;
	CStatic		m_ctlPIC;
	CStatic		m_ctlTitle;
	CButton		m_cbShowImage;
	CListCtrl	m_ctlNGItem;
	CStatic		m_ctlMaster;
	CStatic		m_ctlScan;
	CEdit		m_ctlCSV;
	CEdit		m_ctlTotalNG;
	CEdit		m_ctlAIGood;
	CEdit		m_ctlAIGoodRate;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NG2RAWSNIPPET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAiResult();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickNGItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonNg();
	afx_msg void OnBnClickedButtonNg2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
