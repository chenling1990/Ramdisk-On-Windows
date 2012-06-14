#include "installDlg.h"

// installDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "driverSetup.h"
#include "installDlg.h"
#include "afxdialogex.h"
#include "Windows.h"

// installDlg 对话框

IMPLEMENT_DYNAMIC(installDlg, CDialogEx)

installDlg::installDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(installDlg::IDD, pParent)
	, systemIsXp(FALSE)
	, systemIs32(FALSE)
{

}

installDlg::~installDlg()
{
}

void installDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, systemIsXp);
	DDX_Radio(pDX, IDC_RADIO3, systemIs32);
}


BEGIN_MESSAGE_MAP(installDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &installDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// installDlg 消息处理程序


void installDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//WinExec("ramdisk\\64\\devcon.exe install ramdisk\\64\\win7\\ramdisk.inf ramdisk",SW_SHOW);


	UpdateData(true);
	if(!systemIsXp&&!systemIs32)
		WinExec("ramdisk\\32\\devcon.exe install ramdisk\\32\\xp\\ramdisk.inf ramdisk",SW_SHOW);
	if(systemIsXp&&!systemIs32)
		WinExec("ramdisk\\32\\devcon.exe install ramdisk\\32\\win7\\ramdisk.inf ramdisk",SW_SHOW);
	if(!systemIsXp&&systemIs32)
		WinExec("ramdisk\\64\\devcon.exe install ramdisk\\64\\xp\\ramdisk.inf ramdisk",SW_SHOW);
	if(systemIsXp&&systemIs32)
		WinExec("ramdisk\\64\\devcon.exe install ramdisk\\64\\win7\\ramdisk.inf ramdisk",SW_SHOW);
	CDialogEx::OnOK();
}
