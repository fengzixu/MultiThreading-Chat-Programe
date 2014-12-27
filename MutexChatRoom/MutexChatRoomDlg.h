
// MutexChatRoomDlg.h : 头文件
//

#pragma once

#define WM_RECVDATA WM_USER+1
// CMutexChatRoomDlg 对话框
class CMutexChatRoomDlg : public CDialogEx
{
private:
	SOCKET m_socket;
	//用于接收数据线程传递参数的结构体
	struct RECVPARAM
	{
		SOCKET socket;
		HWND hwnd;
	};
// 构造
public:
	CMutexChatRoomDlg(CWnd* pParent = NULL);	// 标准构造函数
	BOOL InitSocket();
	static DWORD WINAPI RecvProc(LPVOID lpParameter);
// 对话框数据
	enum { IDD = IDD_MUTEXCHATROOM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持 



// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	//对定义的消息相应函数进行声明。
	LRESULT afx_msg OnRecvData(WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangerecvEdit2();
};
