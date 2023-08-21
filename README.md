# asynccurl
A wrapper to use curl with coroutine.

It's still a draft.


```
Task<std::string> fetch(std::string url, asynccurl::Executor &executor) {
  auto buffer = std::make_shared<asynccurl::Write_string>();
  asynccurl::Request_slot req{url};
  req.set_write_buffer(buffer);

  co_await asynccurl::Awaitable_request{req, executor};

  co_return buffer->buffer_;
}

// spawn a task
auto task = fetch(url, exec);
asynccurl::spawn(exec, task);
```

```
// send request in a executor
asynccurl::Executor executor;
executor.run_looping();

// request to stop
executor.request_stop();
```