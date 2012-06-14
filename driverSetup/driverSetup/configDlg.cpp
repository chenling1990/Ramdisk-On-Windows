// configDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "driverSetup.h"
#include "configDlg.h"
#include "afxdialogex.h"


// configDlg 对话框

IMPLEMENT_DYNAMIC(configDlg, CDialogEx)

configDlg::configDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(configDlg::IDD, pParent)
	, diskSymbol(_T(""))
	, diskSize(0)
{

}

configDlg::~configDlg()
{
}

void configDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, diskSymbol);
	DDV_MaxChars(pDX, diskSymbol, 1);
	DDX_Slider(pDX, IDC_SLIDER1, diskSize);
	DDV_MinMaxInt(pDX, diskSize, 1, 100);
}


BEGIN_MESSAGE_MAP(configDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &configDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// configDlg 消息处理程序


void configDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if(diskSymbol.IsEmpty()){
		MessageBox(_T("盘符不能为空"));
		return;
	}
	char sym=diskSymbol[0];


	
	HKEY hKey;
	CString sKeyName;
	LONG lnRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SYSTEM\\CurrentControlSet\\services\\Ramdisk\\Parameters"),0L,KEY_WRITE,&hKey);
	unsigned char DL[]="P\0:\0\0";
	DL[0]=sym;
	CString keyName;
	keyName="DriveLetter";
	
	lnRes=RegSetValueEx(hKey,LPCTSTR(keyName),0,REG_SZ,DL,6);
	
	keyName="DiskSize";

	int DS[256];
	DS[0]=diskSize*1024*1024;

    lnRes=RegSetValueEx(hKey,LPCTSTR(keyName),0,REG_DWORD,(const unsigned char *)DS,4);
	RegCloseKey(hKey);

	CDialogEx::OnOK();
}
