% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/MIXTCOMP_methods.R
\name{plot.MixtComp}
\alias{plot.MixtComp}
\title{Plot of a \emph{MixtComp} object}
\usage{
\method{plot}{MixtComp}(x, nVarMaxToPlot = 3, pkg = c("ggplot2",
  "plotly"), ...)
}
\arguments{
\item{x}{\emph{MixtComp} object}

\item{nVarMaxToPlot}{number of variables to display}

\item{pkg}{"ggplot2" or "plotly". Package used to plot}

\item{...}{Not used.}
}
\description{
Plot of a \emph{MixtComp} object
}
\examples{
\donttest{
data(iris)

# run RMixtComp in unsupervised clustering mode and in basic mode
resLearn <- mixtCompLearn(iris[, -5], nClass = 2:4)

plot(resLearn$res[[2]])
}

}
\seealso{
\code{mixtCompLearn} \code{mixtCompPredict}

Other plot: \code{\link{heatmapClass}},
  \code{\link{heatmapTikSorted}}, \code{\link{heatmapVar}},
  \code{\link{histMisclassif}},
  \code{\link{plotConvergence}}, \code{\link{plotCrit}},
  \code{\link{plotDataBoxplot}}, \code{\link{plotDataCI}},
  \code{\link{plotDiscrimClass}},
  \code{\link{plotDiscrimVar}},
  \code{\link{plotParamConvergence}},
  \code{\link{plotProportion}}
}
\author{
Quentin Grimonprez
}
\concept{plot}
