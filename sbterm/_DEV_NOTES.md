# DEV NOTES

Because I'm developing on the Mac but targeting the RPi I can't install RPi.GPIO with UV.

Instead, in pyproject.toml I include:

```toml
[project.optional-dependencies]
pi = [
    "RPi.GPIO; sys_platform == 'linux' and platform_machine == 'armv7l'"
]
```

On the RPi, I'd use:

```
uv sync --extra pi
```
