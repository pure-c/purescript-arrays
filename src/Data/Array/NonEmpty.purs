module Data.Array.NonEmpty where

import Prelude

import Control.Alt (class Alt)
import Control.Alternative (class Alternative)
import Control.Lazy (class Lazy)
import Control.Monad.Rec.Class (class MonadRec)
import Data.Array as A
import Data.Bifunctor (bimap)
import Data.Eq (class Eq1)
import Data.Foldable (class Foldable)
import Data.FoldableWithIndex (class FoldableWithIndex)
import Data.FunctorWithIndex (class FunctorWithIndex)
import Data.Maybe (Maybe(..), fromJust)
import Data.Ord (class Ord1)
import Data.Traversable (class Traversable)
import Data.TraversableWithIndex (class TraversableWithIndex)
import Data.Tuple (Tuple)
import Data.Unfoldable (class Unfoldable)
import Partial.Unsafe (unsafePartial)

newtype NonEmptyArray a = NonEmptyArray (Array a)

instance showNonEmptyArray :: Show a => Show (NonEmptyArray a) where
  show (NonEmptyArray xs) = "(NonEmptyArray " <> show xs <> ")"

derive newtype instance eqNonEmptyArray :: Eq a => Eq (NonEmptyArray a)
derive newtype instance eq1NonEmptyArray :: Eq1 NonEmptyArray

derive newtype instance ordNonEmptyArray :: Ord a => Ord (NonEmptyArray a)
derive newtype instance ord1NonEmptyArray :: Ord1 NonEmptyArray 


derive newtype instance functorNonEmptyArray :: Functor NonEmptyArray
derive newtype instance functorWithIndexNonEmptyArray :: FunctorWithIndex Int NonEmptyArray

derive newtype instance foldableNonEmptyArray :: Foldable NonEmptyArray
derive newtype instance foldableWithIndexNonEmptyArray :: FoldableWithIndex Int NonEmptyArray

derive newtype instance traversableNonEmptyArray :: Traversable NonEmptyArray
derive newtype instance traversableWithIndexNonEmptyArray :: TraversableWithIndex Int NonEmptyArray

derive newtype instance applyNonEmptyArray :: Apply NonEmptyArray

derive newtype instance applicativeNonEmptyArray :: Applicative NonEmptyArray

derive newtype instance bindNonEmptyArray :: Bind NonEmptyArray

derive newtype instance monadNonEmptyArray :: Monad NonEmptyArray

derive newtype instance altNonEmptyArray :: Alt NonEmptyArray

-- | Internal - adapt an Array transform to NonEmptyArray
adapt :: forall a b. (Array a -> Array b) -> NonEmptyArray a -> NonEmptyArray b
adapt f = NonEmptyArray <<< adaptAny f

-- | Internal - adapt an Array transform with argument to NonEmptyArray
adapt' :: forall a b c. (a -> Array b -> Array c) -> a -> NonEmptyArray b -> NonEmptyArray c
adapt' f = adapt <<< f

-- | Internal - adapt an Array transform with two arguments to NonEmptyArray
adapt'' :: forall a b c d. (a -> b -> Array c -> Array d) -> a -> b -> NonEmptyArray c -> NonEmptyArray d
adapt'' f = adapt' <<< f

-- | Internal - adapt an Array transform to NonEmptyArray,
--   with polymorphic result.
adaptAny :: forall a b. (Array a -> b) -> NonEmptyArray a -> b
adaptAny f = f <<< toArray

-- | Internal - adapt an Array transform with argument to NonEmptyArray,
--   with polymorphic result.
adaptAny' :: forall a b c. (a -> Array b -> c) -> a -> NonEmptyArray b -> c
adaptAny' f = adaptAny <<< f

-- | Internal - adapt an Array transform with two arguments to NonEmptyArray,
--   with polymorphic result.
adaptAny'' :: forall a b c d. (a -> b -> Array c -> d) -> a -> b -> NonEmptyArray c -> d
adaptAny'' f = adaptAny' <<< f

-- | Internal - adapt Array functions returning Maybes to NonEmptyArray
adaptMaybe :: forall a b. (Array a -> Maybe b) -> NonEmptyArray a -> b
adaptMaybe f = unsafePartial $ fromJust <<< f <<< toArray

fromArray :: forall a. Array a -> Maybe (NonEmptyArray a)
fromArray xs
  | A.length xs > 0 = Just (NonEmptyArray xs)
  | otherwise       = Nothing

unsafeFromArray :: forall a. Array a -> NonEmptyArray a
unsafeFromArray = NonEmptyArray

toArray :: forall a. NonEmptyArray a -> Array a
toArray (NonEmptyArray xs) = xs

toUnfoldable :: forall f a. Unfoldable f => NonEmptyArray a -> f a
toUnfoldable = adaptAny A.toUnfoldable

fromFoldable :: forall f a. Foldable f => f a -> Maybe (NonEmptyArray a)
fromFoldable = fromArray <<< A.fromFoldable

singleton :: forall a. a -> NonEmptyArray a
singleton = NonEmptyArray <<< A.singleton

range :: Int -> Int -> NonEmptyArray Int
range x y = NonEmptyArray $ A.range x y

infix 8 range as ..

replicate :: forall a. Int -> a -> Maybe (NonEmptyArray a)
replicate i x
  | i > 0     = Just $ NonEmptyArray $ A.replicate i x
  | otherwise = Nothing

some
  :: forall f a
   . Alternative f
  => Lazy (f (Array a))
  => f a -> f (NonEmptyArray a)
some = map NonEmptyArray <<< A.some

length :: forall a. NonEmptyArray a -> Int
length = adaptAny A.length

cons :: forall a. a -> NonEmptyArray a -> NonEmptyArray a
cons = adapt' A.cons

infixr 6 cons as :

insert :: forall a. Ord a => a -> NonEmptyArray a -> NonEmptyArray a
insert = adapt' A.insert

insertBy :: forall a. (a -> a -> Ordering) -> a -> NonEmptyArray a -> NonEmptyArray a
insertBy = adapt'' A.insertBy

head :: forall a. NonEmptyArray a -> a
head = adaptMaybe A.head

last :: forall a. NonEmptyArray a -> a
last = adaptMaybe A.last

tail :: forall a. NonEmptyArray a -> Array a
tail = adaptMaybe A.tail

init :: forall a. NonEmptyArray a -> Array a
init = adaptMaybe A.init

uncons :: forall a. NonEmptyArray a -> { head :: a, tail :: Array a }
uncons = adaptMaybe A.uncons

unsnoc :: forall a. NonEmptyArray a -> { init :: Array a, last :: a }
unsnoc = adaptMaybe A.unsnoc

index :: forall a. NonEmptyArray a -> Int -> Maybe a
index = adaptAny A.index

infixl 8 index as !!

elemIndex :: forall a. Eq a => a -> NonEmptyArray a -> Maybe Int
elemIndex = adaptAny' A.elemIndex

elemLastIndex :: forall a. Eq a => a -> NonEmptyArray a -> Maybe Int
elemLastIndex = adaptAny' A.elemLastIndex

findIndex :: forall a. (a -> Boolean) -> NonEmptyArray a -> Maybe Int
findIndex = adaptAny' A.findIndex

findLastIndex :: forall a. (a -> Boolean) -> NonEmptyArray a -> Maybe Int
findLastIndex = adaptAny' A.findLastIndex

insertAt :: forall a. Int -> a -> NonEmptyArray a -> Maybe (NonEmptyArray a)
insertAt i x = map NonEmptyArray <<< A.insertAt i x <<< toArray

deleteAt :: forall a. Int -> NonEmptyArray a -> Maybe (Array a)
deleteAt = adaptAny' A.deleteAt

updateAt :: forall a. Int -> a -> NonEmptyArray a -> Maybe (NonEmptyArray a)
updateAt i x = map NonEmptyArray <<< A.updateAt i x <<< toArray

modifyAt :: forall a. Int -> (a -> a) -> NonEmptyArray a -> Maybe (NonEmptyArray a)
modifyAt i f = map NonEmptyArray <<< A.modifyAt i f <<< toArray

alterAt :: forall a. Int -> (a -> Maybe a) -> NonEmptyArray a -> Maybe (Array a)
alterAt i f = A.alterAt i f <<< toArray

reverse :: forall a. NonEmptyArray a -> NonEmptyArray a
reverse = adapt A.reverse

concat :: forall a. NonEmptyArray (NonEmptyArray a) -> NonEmptyArray a
concat = NonEmptyArray <<< A.concat <<< toArray <<< map toArray

concatMap :: forall a b. (a -> NonEmptyArray b) -> NonEmptyArray a -> NonEmptyArray b
concatMap = flip bind

filter :: forall a. (a -> Boolean) -> NonEmptyArray a -> Array a
filter = adaptAny' A.filter

partition
  :: forall a
   . (a -> Boolean)
  -> NonEmptyArray a
  -> { yes :: Array a, no :: Array a}
partition = adaptAny' A.partition

filterA
  :: forall a f
   . Applicative f
  => (a -> f Boolean)
  -> NonEmptyArray a
  -> f (Array a)
filterA = adaptAny' A.filterA

mapMaybe :: forall a b. (a -> Maybe b) -> NonEmptyArray a -> Array b
mapMaybe = adaptAny' A.mapMaybe

catMaybes :: forall a. NonEmptyArray (Maybe a) -> Array a
catMaybes = adaptAny A.catMaybes

updateAtIndices :: forall t a. Foldable t => t (Tuple Int a) -> NonEmptyArray a -> NonEmptyArray a
updateAtIndices = adapt' A.updateAtIndices

modifyAtIndices :: forall t a. Foldable t => t Int -> (a -> a) -> NonEmptyArray a -> NonEmptyArray a
modifyAtIndices = adapt'' A.modifyAtIndices

sort :: forall a. Ord a => NonEmptyArray a -> NonEmptyArray a
sort = adapt A.sort

sortBy :: forall a. (a -> a -> Ordering) -> NonEmptyArray a -> NonEmptyArray a
sortBy = adapt' A.sortBy

sortWith :: forall a b. Ord b => (a -> b) -> NonEmptyArray a -> NonEmptyArray a
sortWith = adapt' A.sortWith

slice :: forall a. Int -> Int -> NonEmptyArray a -> Array a
slice = adaptAny'' A.slice

take :: forall a. Int -> NonEmptyArray a -> Array a
take = adaptAny' A.take

takeEnd :: forall a. Int -> NonEmptyArray a -> Array a
takeEnd = adaptAny' A.takeEnd

takeWhile :: forall a. (a -> Boolean) -> NonEmptyArray a -> Array a
takeWhile = adaptAny' A.takeWhile

drop :: forall a. Int -> NonEmptyArray a -> Array a
drop = adaptAny' A.drop

dropEnd :: forall a. Int -> NonEmptyArray a -> Array a
dropEnd = adaptAny' A.dropEnd

dropWhile :: forall a. (a -> Boolean) -> NonEmptyArray a -> Array a
dropWhile = adaptAny' A.dropWhile

span
  :: forall a
   . (a -> Boolean)
  -> NonEmptyArray a
  -> { init :: Array a, rest :: Array a }
span = adaptAny' A.span

--TODO
--group :: forall a. Eq a => NonEmptyArray a -> NonEmptyArray (NonEmptyArray a)

--TODO
--group' :: forall a. Ord a => NonEmptyArray a -> NonEmptyArray (NonEmptyArray a)

--TODO
--groupBy :: forall a. (a -> a -> Boolean) -> NonEmptyArray a -> NonEmptyArray (NonEmptyArray a)

nub :: forall a. Eq a => NonEmptyArray a -> NonEmptyArray a
nub = adapt A.nub

nubBy :: forall a. (a -> a -> Boolean) -> NonEmptyArray a -> NonEmptyArray a
nubBy = adapt' A.nubBy

delete :: forall a. Eq a => a -> NonEmptyArray a -> Array a
delete = adaptAny' A.delete

deleteBy :: forall a. (a -> a -> Boolean) -> a -> NonEmptyArray a -> Array a
deleteBy = adaptAny'' A.deleteBy

zipWith
  :: forall a b c
   . (a -> b -> c)
  -> NonEmptyArray a
  -> NonEmptyArray b
  -> NonEmptyArray c
zipWith f xs ys = NonEmptyArray $ A.zipWith f (toArray xs) (toArray ys)


zipWithA
  :: forall m a b c
   . Applicative m
  => (a -> b -> m c)
  -> NonEmptyArray a
  -> NonEmptyArray b
  -> m (NonEmptyArray c)
zipWithA f xs ys = NonEmptyArray <$> A.zipWithA f (toArray xs) (toArray ys)

zip :: forall a b. NonEmptyArray a -> NonEmptyArray b -> NonEmptyArray (Tuple a b)
zip xs ys = NonEmptyArray $ toArray xs `A.zip` toArray ys

unzip :: forall a b. NonEmptyArray (Tuple a b) -> Tuple (NonEmptyArray a) (NonEmptyArray b)
unzip = bimap NonEmptyArray NonEmptyArray <<< A.unzip <<< toArray

foldM :: forall m a b. Monad m => (a -> b -> m a) -> a -> NonEmptyArray b -> m a
foldM = adaptAny'' A.foldM

foldRecM :: forall m a b. MonadRec m => (a -> b -> m a) -> a -> NonEmptyArray b -> m a
foldRecM = adaptAny'' A.foldRecM

unsafeIndex :: forall a. Partial => NonEmptyArray a -> Int -> a
unsafeIndex = adaptAny A.unsafeIndex
