#pragma once


// removeDlg 对话框

class removeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(removeDlg)

public:
	removeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~removeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Remove };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
};
