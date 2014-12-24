
// MutexChatRoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MutexChatRoom.h"
#include "MutexChatRoomDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define MAXN 100000
#define WM_RECVDATA WM_USER+1

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMutexChatRoomDlg 对话框



CMutexChatRoomDlg::CMutexChatRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMutexChatRoomDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMutexChatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMutexChatRoomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMutexChatRoomDlg 消息处理程序

BOOL CMutexChatRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	InitSocket();

	RECVPARAM *pRecvParam = new RECVPARAM;
	//要传递给接受数据线程的套接字和窗口句柄

	pRecvParam->socket=m_socket;
	//所有与窗口有关的类都有一个数据成员，m_hWnd，他保存了与该类相关的窗口句柄
	pRecvParam->hwnd = m_hWnd;

	//创建接收线程
	//线程相应函数是在创建线程函数执行的时候自动调用的，所以不能创建为一个成员函数
	//因为成员函数是必须要用对象来调用的。为了保证面向对象编程的封装性，可以将这样的函数声明为静态成员函数
	//静态成员的函数只属于类，不属于任何对象。
	//之所以用新的子线程来执行接受数据，就是因为接受数据的函数如果没有接受到数据会阻塞，使得整个程序挂起，停止运行，聊天程序即使接收端也是发送端，任何一部分的功能都不能影响到另外一部分
	HANDLE hThread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);
	CloseHandle(hThread);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMutexChatRoomDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMutexChatRoomDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMutexChatRoomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMutexChatRoomDlg::InitSocket()
{
	//创建套接字，使用UDP协议中的数据报套接字
	m_socket = socket(AF_INET,SOCK_DGRAM,0);

	//检查套接字是否创建失败
	if(INVALID_SOCKET == m_socket) 
	{
		MessageBox("套接字创建失败");
		return FALSE;
	}
	//
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(6000);
	//将ip地址设为所有分配给这个机器的ip地址，也就是说，可以通过所有分配给这个机器的ip地址来接受和发送数据
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval;
	//绑定端口
	retval = bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	if(retval == SOCKET_ERROR)
	{
		closesocket(m_socket);
		MessageBox("绑定失败");
		return FALSE;
	}
	return TRUE;

}

DWORD WINAPI CMutexChatRoomDlg::RecvProc(LPVOID lpParameter)
{
	//获取传递进来的函数
	SOCKET sock = ((RECVPARAM*)lpParameter)->socket;
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd;
	delete lpParameter;

	//主要用来存储发送方绑定的地址信息
	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);
	char recvbuff[MAXN] = {0};
	char tempbuff[MAXN] = {0};

	int retval;

	//因为要一直准备在接受数据，所以，接受数据主要功能代码要不断的循环执行
	while(TRUE)
	{
		//接受发来的数据
		retval = recvfrom(sock,recvbuff,MAXN,0,(SOCKADDR*)&addrFrom,&len);
		if(SOCKET_ERROR == retval)
		{
			break;
		}
		//格式化存储输入到tempbuff数组里面
		sprintf(tempbuff,"%s说： %s",inet_ntoa(addrFrom.sin_addr),0,(LPARAM)recvbuff);

		::PostMessage(hwnd,WM_RECVDATA,0,(LPARAM)tempbuff);
	}
}