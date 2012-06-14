#pragma once


// installDlg 对话框

class installDlg : public CDialogEx
{
	DECLARE_DYNAMIC(installDlg)

public:
	installDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~installDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Install };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL systemIsXp;
	BOOL systemIs32;
	afx_msg void OnBnClickedOk();
};
