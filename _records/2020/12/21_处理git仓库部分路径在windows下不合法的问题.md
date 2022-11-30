



```sh
fp="$1"
new_fp=${fp//"|"/""}
mv "$fp" "$new_fp"
for ext in .html .htm; do
  files=${fp%${ext}}_files
  if [[ -d "$files" ]]; then
    new_files=${files//"$rep"/""}
    mv "$files" "${new_files}"
    break
  fi
done
# 对于path里有 ' 的还是有问题，以后再看
```



```sh

fp="$1"
rep="$2"
new_fp=${fp//"$rep"/""}
mv "$fp" "$new_fp"
for ext in .html .htm; do
  files=${fp%${ext}}_files
  if [[ -d "$files" ]]; then
    new_files=${files//"$rep"/""}
    mv "$files" "${new_files}"
    break
  fi
done
```







```sh
find ./ -name "*|*" | xargs bash /tmp/process_pipe.sh -I'{}' '{}' '|'
find ./ -name "*:*" | xargs bash /tmp/process_pipe.sh -I'{}' '{}' ':'
find ./ -name "*\"*" | xargs bash /tmp/process_pipe.sh -I'{}' '{}' '"'
# 有点问题

find ./ -name "*'*" -print0 | xargs -0 -I'{}' bash /tmp/process_pipe.sh '{}' "'"

s=`ls | grep "^Go Data" | head -1 | cut -c 19-`
echo "$s"  | od -A n -t x1  #  ef 80 ba 20 0a
t=`echo -e '\xef\x80\xba\x20'`
find ./ -name "*${t}*" -print0 | xargs -0 -I'{}' bash /tmp/process_pipe.sh '{}' "$t"
```



```sh
# 20 ef 81 bc 20
# 20 20

t=`echo -e '\xef\x81\xbc'`
find ./ -name "*$t*" | head
find ./ -name "*${t}*" -print0 | xargs -0 -I'{}' bash /tmp/process_pipe.sh '{}' "$t"
```



```sh
find ./ -name "*\?*" | head
find ./ -name "*\?*" -print0 | xargs -0 -I'{}' bash /tmp/process_pipe.sh '{}' "\?"
# 好像替换时有问题，后来手工完成的
```

