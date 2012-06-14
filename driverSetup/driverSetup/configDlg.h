#pragma once


// configDlg 对话框

class configDlg : public CDialogEx
{
	DECLARE_DYNAMIC(configDlg)

public:
	configDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~configDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Config };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString diskSymbol;
	int diskSize;
	afx_msg void OnBnClickedOk();
};
