#pragma once

/* Generic Device and Driver Name generator */
	/*
		#define myname "MyExample"

		MY_DEVICE_NAME becomes "MyExampleDevice"
		MY_DRIVER_NAME becomes "MyExampleDriver"	

		DEVICE_NAME becomes L"\\Device\\MyExampleDevice"
		SYMLINK_NAME becomes L"\\??\\MyExampleDriver"
		CLIENT_DEVICE becomes L"\\\\.\\MyExampleDriver"
	*/


	#define WIDE2(x) L ## x
	#define WIDE(x) WIDE2(x)

	#define name "EDR"
	#define MY_DEVICE_NAME name "Device"
	#define MY_DRIVER_NAME name "Driver"

	#define DEVICE_NAME L"\\Device\\" WIDE(name) L"Device"
	#define SYMLINK_NAME L"\\??\\" WIDE(name) L"Driver"

	#define CLIENT_DEVICE L"\\\\.\\" WIDE(name) L"Driver"