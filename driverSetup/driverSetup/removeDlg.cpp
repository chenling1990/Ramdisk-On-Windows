// removeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "driverSetup.h"
#include "removeDlg.h"
#include "afxdialogex.h"
#include "Windows.h"

// removeDlg 对话框

IMPLEMENT_DYNAMIC(removeDlg, CDialogEx)

removeDlg::removeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(removeDlg::IDD, pParent)
{

}

removeDlg::~removeDlg()
{
}

void removeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(removeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &removeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &removeDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// removeDlg 消息处理程序


void removeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	WinExec("ramdisk\\32\\devcon.exe remove ramdisk",SW_SHOW);
	MessageBox(_T("卸载成功！"));

	CDialogEx::OnOK();
}


void removeDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	WinExec("ramdisk\\64\\devcon.exe remove ramdisk",SW_SHOW);
	MessageBox(_T("卸载成功！"));

	CDialogEx::OnOK();

}
