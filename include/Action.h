#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>

class Session;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
	BaseAction();
	ActionStatus getStatus() const;
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
    virtual BaseAction* clone()=0;
    virtual std::string getStatusTo_String() const;
    virtual ~BaseAction(); //destructor
protected:
	void complete();
	void error(const std::string& errorMsg);
	std::string getErrorMsg() const;
private:
	std::string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser(std::string username, std::string tlc);
	virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();

private:
    std::string user_name;
    std::string three_letter_code;

};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(std::string changeToUser);
	virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();

private:
    std::string change_To_User;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string username);//constructor
	virtual void act(Session & sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();

private:
    std::string user_name;
};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string original_user_name, std::string new_user_name);
	virtual void act(Session & sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();

private:
    std::string original_user_name;
    std::string new_user_name;
};

class PrintContentList : public BaseAction {
public:
    PrintContentList();
	virtual void act (Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();
	virtual void act (Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};


class Watch : public BaseAction {
public:
	Watch(long content_id);//constructor
    virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    long id;
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();//constructor
	virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};

class Exit : public BaseAction {
public:
    Exit();//constructor
	virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();

};
#endif
