#include "filesinfo.h"

QList<QString> intersectKeys(const QList<QString>& keys_1, const QList<QString>& keys_2)
{
    // преобразовывваем списки в множества, для выполнения операции пересечения
    QSet<QString> keys_1_set = keys_1.toSet();
    QSet<QString> keys_2_set = keys_2.toSet();

    QList<QString> keys = (keys_1_set.intersect(keys_2_set)).toList();

    return keys;
}
