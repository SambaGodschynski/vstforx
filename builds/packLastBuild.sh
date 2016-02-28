if [ $# -ne 1 ]
then
  echo 'usage: $0 mac|win|win64'
  exit 1
fi

V=$(cat currVersion.txt | grep -o "[0-9A-Za-z.]*")
rm -rf $1/*
sh pack.sh $1 $V
