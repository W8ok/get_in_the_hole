// utils.h
#pragma once

#include <stdlib.h>
#include <time.h>

#include <SDL3/SDL.h>

// Dont ask
#define CAST(to, from) (*(to*)&(union{__typeof__(from) src; to dst;}){from}.dst)

#define ERROR_EXIT(fmt, ...) \
	do { \
		time_t now = time(NULL); \
		struct tm *tm_info = localtime(&now); \
		char time_buffer[20]; \
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info); \
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[%s] [%s:%d %s()] " fmt, time_buffer, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while(0)

#define ERROR_RETURN(fmt, ...) \
	do { \
		time_t now = time(NULL); \
		struct tm *tm_info = localtime(&now); \
		char time_buffer[20]; \
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info); \
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] [%s:%d %s()] " fmt, time_buffer, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
		return false; \
	} while(0)

#define LOG_WARN(fmt, ...) \
	do { \
		time_t now = time(NULL); \
		struct tm *tm_info = localtime(&now); \
		char time_buffer[20]; \
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info); \
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] " fmt, time_buffer, ##__VA_ARGS__); \
	} while(0)

#define LOG_INFO(fmt, ...) \
	do { \
		time_t now = time(NULL); \
		struct tm *tm_info = localtime(&now); \
		char time_buffer[20]; \
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info); \
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] " fmt, time_buffer, ##__VA_ARGS__); \
	} while(0)

#define LOG_DEBUG(fmt, ...) \
	do { \
		time_t now = time(NULL); \
		struct tm *tm_info = localtime(&now); \
		char time_buffer[20]; \
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tm_info); \
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DEBUG: [%s] " fmt, time_buffer, ##__VA_ARGS__); \
	} while(0)

#define DA_APPEND(array, x) \
	do { \
		if (array.count >= array.capacity) \
		{ \
			/*if (array.capacity == 0)*/ \
			/*	array.capacity = 1;*/ \
			/*else*/ \
			/*	array.capacity *= 2;*/ \
			array.capacity++; \
			void* new_data = SDL_realloc(array.data, array.capacity * sizeof(*array.data)); \
			if (!new_data) \
				ERROR_RETURN("Memory Allocation Failure"); \
			array.data = new_data; \
		} \
		array.data[array.count++] = x; \
	} while(0)
