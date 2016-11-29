#ifndef TASKBAR_H_
#define TASKBAR_H_

class Taskbar{
	public:
		static void Init();
		static void Destroy();
		static void Render();
		static void Handle();
		static int GetHeight();
};
#endif
