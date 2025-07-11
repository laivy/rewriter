import os
import sys
from pathlib import Path


def remove_all_symlinks(path: Path, non_symlink_files: list[Path]):
    if not path.exists():
        return

    for item in path.iterdir():
        if item.is_file():
            if item.is_symlink():
                item.unlink()
            elif item.name != ".gitkeep":
                non_symlink_files.append(item)
        elif item.is_dir():
            remove_all_symlinks(item, non_symlink_files)
            if not non_symlink_files:
                item.rmdir()


def make_symlink_recursive(src_dir: Path, dst_dir: Path):
    for item in src_dir.iterdir():
        if item.is_file():
            dst_file: Path = dst_dir / item.name
            if dst_file.name != ".gitkeep":
                os.symlink(item.resolve(), dst_file, item.is_dir())
        elif item.is_dir():
            dst_subdir: Path = dst_dir / item.name
            dst_subdir.mkdir(parents=True, exist_ok=True)
            make_symlink_recursive(item, dst_subdir)


def main():
    # 명령행 인자 유효성 체크
    if len(sys.argv) < 3:
        msg: str = "Invalid command line argument"
        for i, arg in enumerate(sys.argv):
            msg += f"\n{i}: \"{arg}\""
        sys.exit(msg)

    # Bin/.../Data 폴더 삭제 및 생성
    dst_dir: Path = Path(sys.argv[1])
    non_symlink_files: list[Path] = list()
    remove_all_symlinks(dst_dir, non_symlink_files)
    if non_symlink_files:
        msg: str = "Symlink Fail\n"
        msg += "Non-symlink file exists"
        for file in non_symlink_files:
            msg += f"\n    {file}"
        sys.exit(msg)
    dst_dir.mkdir(parents=True, exist_ok=True)

    # 링크
    src_dirs: list[Path] = [Path(arg) for arg in sys.argv[2:]]
    for src_dir in src_dirs:
        make_symlink_recursive(src_dir, dst_dir)

    # 결과
    msg: str = "Symlink Success\n"
    msg += f"Source:\n"
    for src_dir in src_dirs:
        msg += f"    {src_dir}\n"
    msg += "Destination:\n"
    msg += f"    {dst_dir}"
    print(msg)


if __name__ == '__main__':
    main()
